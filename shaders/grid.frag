#version 400 core
in vec3 FragPos;

out vec4 FragColor;

void main() {
  vec2 uv = FragPos.xz;
  vec2 uvDeriv = fwidth(uv);
  vec2 lineWidth = vec2(0.02);
  vec2 drawWidth = max(lineWidth, uvDeriv);

  vec2 lineAA = uvDeriv * 1.5;

  vec2 gridUV = 1.0 - abs(fract(uv) * 2.0 - 1.0);
  vec2 grid2 = smoothstep(drawWidth + lineAA, drawWidth - lineAA, gridUV);
  grid2 *= clamp(lineWidth / drawWidth, 0.0, 1.0);
  float grid = mix(grid2.x, 1.0, grid2.y);

  vec3 color = vec3(grid);

  FragColor = vec4(color, 1.0);
}
