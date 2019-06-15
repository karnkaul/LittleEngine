void main()
{
    /* Update transform */
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    /* Set uv */
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    /* Set colour */
    gl_FrontColor = gl_Color;
}
