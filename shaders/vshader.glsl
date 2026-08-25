// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform mat4 mvp_matrix;

attribute vec3 a_position;
attribute vec3 a_normal;
//attribute vec2 a_texcoord;

varying vec2 v_texcoord;

varying vec3 Normal;
varying vec3 FragPos;

void main()
{
    // Calculate vertex position in screen space
    gl_Position = (projection * view /* * model */) * vec4(a_position, 1.0);
    //v_texcoord = a_texcoord;
    Normal = a_normal;
    FragPos = a_position;
}
