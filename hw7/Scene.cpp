//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TODO Implement Path Tracing Algorithm here
    // Implementation of Path Tracing
    Intersection intersection_p = Scene::intersect(ray);

    if(!intersection_p.happened) {
        return Vector3f(0,0,0);
    }

    // get point info
    Vector3f p = intersection_p.coords;
    Vector3f N = intersection_p.normal;
    Vector3f Wo = ray.direction;
    Material* m_p = intersection_p.m;

    // init L_dir
    Vector3f L_dir(0.0);

    // Uniformly sample the light at x (pdf_light = 1 / A)
    Intersection inner;
    float pdf_light;
    sampleLight(inner, pdf_light);

    // get inner info
    Vector3f x = inner.coords;
    Vector3f Ws = (x - p).normalized();
    Vector3f NN = inner.normal;
    Vector3f emit = inner.emit;

    // Shoot a ray from p to x
    Ray ray_p_x(p + EPSILON * N, Ws); // avoid self-block
    Intersection intersection_p_x = Scene::intersect(ray_p_x);

    // If the ray is not blocked in the middle, the intersection_p_x is the light source
    if(intersection_p_x.happened && intersection_p_x.m->hasEmission()) {
        L_dir = emit * m_p->eval(Wo, Ws, N) * dotProduct(Ws, N) * dotProduct(-Ws, NN) / pow((x - p).norm(), 2) / pdf_light;
    }

    // init L_indir
    Vector3f L_indir(0.0);

    // // Test Russian Roulette with probability RussianRoulette
    if(get_random_float() <= RussianRoulette) {
        // sample the hemisphere toward wi (pdf_hemi = 1 / 2pi)
        Vector3f Wi = m_p->sample(Wo, N).normalized();

        // Trace a ray r (p, wi)
        Ray r(p, Wi);

        // If ray r hit a non-emitting object at q
        Intersection intersection_q = Scene::intersect(r);
        if(intersection_q.happened && !intersection_q.m->hasEmission()) {
            L_indir = castRay(r, depth + 1) * m_p->eval(Wo, Wi, N) * dotProduct(Wi, N) / m_p->pdf(Wo, Wi, N) / RussianRoulette;
        }
    }

    return m_p->getEmission() + L_dir + L_indir;
}