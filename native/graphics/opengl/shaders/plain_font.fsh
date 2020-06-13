#version #opengl_version

#include "color_uniforms.glsl"

varying vec3 tpos;

void main()
{
    float alphaV = 1.0;
    if ( tpos.z < 0.5 ) {
        vec2 p = tpos.xy;
        // Gradients
        vec2 px = dFdx(p);
        vec2 py = dFdy(p);
        // Chaattribute rule
        float fx = ((2.0*p.x)*px.x - px.y);
        float fy = ((2.0*p.x)*py.x - py.y);
        // Signed distance
        float dist = fx*fx + fy*fy;
        float sd = (p.x*p.x - p.y)/sqrt(dist);
        // Linear alpha
        if ( sd > 0.5 ) {
            alphaV *= clamp(0.5 - sd, 0.0, 1.0);
        }
    } else {
        // ###WEBGL1###
        // removed round
//        alphaV *= round((tpos.x * tpos.x - tpos.y) * tpos.z + 0.5);
        alphaV *= (tpos.x * tpos.x - tpos.y) * tpos.z + 0.5;
    }
    float aFactor = alpha*alphaV*opacity;
    gl_FragColor = vec4(diffuseColor * aFactor, aFactor);
}
