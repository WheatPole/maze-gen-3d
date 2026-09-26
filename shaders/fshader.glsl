// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
varying vec3 Normal;
varying vec3 FragPos;

uniform sampler2D texture;

//varying vec2 v_texcoord;
uniform vec4 objectColor;
uniform vec4 lightColor;
uniform vec4 lightPos;

uniform vec4 selectionStart;
uniform vec4 selectionEnd;
uniform vec4 selectionColor;
//vec4 fog

void main()
{
    // Set fragment color from texture
    //gl_FragColor = texture2D(texture, v_texcoord);
    //gl_FragColor = sampleColor;
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor.xyz;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos.xyz - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor.xyz;

    vec3 result = (ambient + diffuse)/* * sampleColor.xyz*/;
    gl_FragColor = vec4(result, 1.0) * objectColor/* * texture2D(texture, v_texcoord) */;
    if (selectionStart.x - 1e-5 <= FragPos.x
            && selectionStart.y - 1e-5 <= FragPos.y
            && selectionStart.z - 1e-5 <= FragPos.z
            && selectionEnd.x + 1e-5 >= FragPos.x
            && selectionEnd.y + 1e-5 >= FragPos.y
            && selectionEnd.z + 1e-5 >= FragPos.z) {
        gl_FragColor = gl_FragColor * selectionColor/* * texture2D(texture, v_texcoord) */;
    }
    //gl_FragColor *= vec4(FragPos.x / 10, FragPos.y/10, FragPos.z/10, 1.0);

}
