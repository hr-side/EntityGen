#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform float Grid_size_;
uniform vec2 Canvas_size;

out vec4 finalColor;

const vec4 Color_A = vec4(0.8, 0.8, 0.8, 1.0);
const vec4 Color_B = vec4(0.6, 0.6, 0.6, 1.0);

void main() {
    float Grid_size = (Grid_size_ <= 0) ? 8 : Grid_size_;

    float pixelX = fragTexCoord.x * Canvas_size.x;
    float pixelY = fragTexCoord.y * Canvas_size.y;

    int Cell_X = int(pixelX / Grid_size);
    int Cell_Y = int(pixelY / Grid_size);

    if ((Cell_X + Cell_Y) % 2 == 0)
    {
        finalColor = Color_A;
    }
    else
    {
        finalColor = Color_B;
    }
}
