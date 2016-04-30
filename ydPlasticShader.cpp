#include <ai.h>
#include <cstring>

// A Simple Shader.
// testing by chi-chang chu.
AI_SHADER_NODE_EXPORT_METHODS(SimpleMethods);
 
namespace
{
 
enum SimpleParams { 

    p_Kd_color,
    p_Ks_color,      
    p_roughness,
    p_specbalance };

 
};
 
node_parameters
{
    AiParameterRGB("Kd_color", 1.0f, 0.7f, 0.7f);
    AiParameterRGB("Ks_color", 0.7f, 0.7f, 0.7f);
    AiParameterFLT("roughness", 0.2f);
    AiParameterFLT("specbalance", 0.1f);
}
 
node_initialize
{
}
 
node_update
{
}
 
node_finish
{
}
 

shader_evaluate
{
    //initial input param for evaluation
    AtColor Kd = AiShaderEvalParamRGB(p_Kd_color);
    AtColor Ks = AiShaderEvalParamRGB(p_Ks_color);
    float roughness = AiShaderEvalParamFlt(p_roughness);
    float specbalance = AiShaderEvalParamFlt(p_specbalance);

    //create accumulator variables for calcalation.
    AtColor Dda, Dsa, IDd, IDs;

    //initalize accumulators to 0 (black)
    Dda = Dsa = IDd = IDs = AI_RGB_BLACK;

    //create pointers...
    void *spec_data = AiWardDuerMISCreateData(sg, NULL, NULL, roughness, roughness);
    /*
    Returns a pointer to the data required by AiEvaluateLightSample() to sample the Ward-Duer BRDF using multiple importance sampling.
    The returned data is allocated in a pixel pool and does not need to be freed by the caller.
    Parameters
    sg  shader globals context
    u   surface tangent vector along the U coordinate
    v   surface tangent vector along the V coordinate
    rx  specular roughness along the U direction
    ry  specular roughness along the V direction
    */
    float diffuse_roughness = 0.0f;
    void *diffuse_data = AiOrenNayarMISCreateData(sg, diffuse_roughness);
    //Returns a pointer to the data required by AiEvaluateLightSample() to sample the Oren-Nayar BRDF using multiple importance sampling.

    AiLightsPrepare(sg);

    while(AiLightsGetSample(sg)) // loop through light to compute DIRECT effects ...
    {
        //direct specular
        if (AiLightGetAffectSpecular(sg->Lp)) //pointer to current light node
        {
            //eveluate ...
            Dsa += AiEvaluateLightSample(sg, spec_data, AiWardDuerMISSample, AiWardDuerMISBRDF, AiWardDuerMISPDF) * specbalance;
        }

        //direct diffuse
        if (AiLightGetAffectDiffuse(sg->Lp)) //pointer to current light node
        {
            //eveluate ...
            Dda += AiEvaluateLightSample(sg, diffuse_data, AiOrenNayarMISSample, AiOrenNayarMISBRDF, AiOrenNayarMISPDF) * (1 - specbalance);
        }
    }
    // INDIRECT effects ...
    //indirect specular
    IDs = AiWardDuerIntegrate(&sg->Nf, sg, &sg->dPdu, &sg->dPdv, roughness, roughness) * specbalance; 
    //surface derivative wrt U,V parameter (not normalized) 
    /*
    Returns the reflected indirect-radiance with an improved Ward-Duer BRDF.
    Computes the reflected indirect-radiance integrated over the hemisphere and weighted by the physically plausible, anisotropic Ward-Duer BRDF. The reflected radiance originates at sg->P and goes along the sg->Rd direction.
    Use this for soft, blurred reflections, aka "glossy" reflection.
    Note
    NULL values for u and v may be provided. However if the caller provides non-NULL vectors, it is expected that they form an orthonormal basis with N. It is also expected that all vectors be of unit length.
    Parameters
    N   normal vector that defines the hemisphere of incoming radiance
    sg  shader globals context where reflected radiance is coming from
    u   surface tangent vector along the U direction
    v   surface tangent vector along the V direction
    rx  specular roughness along the U direction, in [0,1]
    ry  specular roughness along the V direction, in [0,1]
    Returns
    reflected radiance along the viewing direction sg->Rd //ray direction 
    */
    IDd = AiOrenNayarIntegrate(&sg->Nf, sg, diffuse_roughness) * (1 - specbalance);
    /*
    Returns the reflected indirect-radiance with an Oren-Nayar BRDF.
    Computes the reflected indirect-radiance integrated over the hemisphere and weighted by the Oren-Nayar BRDF. 
    The reflected radiance originates at sg->P and goes along the sg->Rd direction.  //shading point in world-space
    Use this as a view-dependant alternative to Lambert-based diffuse global illumination.

    Parameters
    N   normal vector that defines the hemisphere of incoming radiance
    sg  shader globals context where reflected radiance is coming from
    r   surface roughness, normalized in the [0, 1] range
    Returns
    reflected radiance along the viewing direction sg->Rd
    */

    //add up indirect and direct contributions...
    sg->out.RGB = Kd * (Dda + IDd) + Ks * (Dsa + IDs);

}

node_loader
{
    if (i > 0) return false;
 
    node->methods        = SimpleMethods;
    node->output_type    = AI_TYPE_RGB;
    node->name           = "ydPlasticShader";
    node->node_type      = AI_NODE_SHADER;
    strcpy(node->version, AI_VERSION);
    return true;
}

//export ARNOLD_PATH=/X/tools/binlinux/arnold/arnold-4.1.3.3

//g++ -o ydPlasticShader.os -c -fPIC -D_LINUX -I$ARNOLD_PATH/include ydPlasticShader.cpp
//g++ -o ydPlasticShader.so -shared ydPlasticShader.os -L$ARNOLD_PATH/bin -lai

//$ARNOLD_PATH/bin/kick -l /USERS/chichang/tools/cpp/al_shaders/ydPlasticShader.os -info simple

//export ARNOLD_PLUGIN_PATH=/USERS/chichang/tools/cpp/al_shaders
//export MTOA_TEMPLATES_PATH=/USERS/chichang/tools/cpp/al_shaders

//export ARNOLD_PLUGIN_PATH=/USERS/chichang/tools/cpp/al_shaders/jf-nested-dielectric-master/binaries/linux/arnold_4.1.3.3
//export MTOA_TEMPLATES_PATH=/USERS/chichang/tools/cpp/al_shaders/jf-nested-dielectric-master/binaries/linux/arnold_4.1.3.3