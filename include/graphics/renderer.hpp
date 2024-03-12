#include <graphics/camera.hpp>
#include <graphics/shader.hpp>
#include <image/image.hpp>

class Renderer {

public:
    Renderer(Camera* cam);
    ~Renderer();

    void renderImage(Image& image);

    void setShader(Shader* shad) { shader = shad; }

private:
    Camera* camera;
    Shader* shader;
};
