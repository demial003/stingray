#version 400 core
in vec3 FragPos;

out vec4 FragColor;

void main() {
  vec2 uv = FragPos.xz;
  vec2 uvDeriv = fwidth(uv);

  float lineWidth = 0.02;
  vec2 lineAA = 1.5 * uvDeriv;

  vec2 gridUV = 1.0 - abs(fract(uv) * 2.0 - 1.0);
  vec2 grid2 = smoothstep(lineWidth + lineAA, lineWidth - lineAA, gridUV);
  float grid = mix(grid2.x, 1.0, grid2.y);

  vec3 color = vec3(grid);

  FragColor = vec4(color, 1.0);
}
