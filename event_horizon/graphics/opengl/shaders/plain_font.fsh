#version #opengl_version
       uniform vec3   diffuseColor;
       uniform float  alpha;
       uniform float  opacity;

       in vec3 tpos;
       out vec4 FragColor;

       void main()
       {
           float alphaV = 1.0;
           if (tpos.z != 0.0)
           {
               vec2 p = tpos.xy;
               // Gradients
               vec2 px = dFdx(p);
               vec2 py = dFdy(p);
               // Chain rule
               float fx = ((2.0*p.x)*px.x - px.y);
               float fy = ((2.0*p.x)*py.x - py.y);
               // Signed distance
               float dist = fx*fx + fy*fy;
               float sd = (p.x*p.x - p.y)*-tpos.z/sqrt(dist);
               // Linear alpha
               alphaV = clamp(0.5 - sd, 0.0, 1.0);
           }
           FragColor = vec4(diffuseColor, alpha*alphaV*opacity);
       }
    