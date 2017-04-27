#include <fstream>
#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "CGL/matrix3x3.h"
#include "CGL/misc.h"

#include "math.h"

#include "particle.h"
#include "grid.h"
#include "mesh.h"
#include "constants.h"

#define STR(A) #A

class Camera {
 public:
  /*
    Sets the field of view to match screen screenW/H.
    NOTE: data and screenW/H will almost certainly disagree about the aspect
          ratio. screenW/H are treated as the source of truth, and the field
          of view is expanded along whichever dimension is too narrow.
    NOTE2: info.hFov and info.vFov are expected to be in DEGREES.
  */
  void configure(size_t screenW, size_t screenH);


  void place(const Vector3D& targetPos, const double phi, const double theta,
             const double r, const double minR, const double maxR);

  std::string param_string() {
    return "";
  }

  void set_screen_size(const size_t screenW, const size_t screenH);

  /*
    Translates the camera such that a value at distance d directly in front of
    the camera moves by (dx, dy). Note that dx and dy are in screen coordinates,
    while d is in world-space coordinates (like pos/dir/up).
  */
  void move_by(const double dx, const double dy, const double d);

  /*
    Move the specified amount along the view axis.
  */
  void move_forward(const double dist);

  /*
    Rotate by the specified amount around the target.
  */
  void rotate_by(const double dPhi, const double dTheta);
  double viewDist;
  Vector3D position() const { return pos; }
  Vector3D view_point() const { return targetPos; }
  Vector3D up_dir() const { return c2w[1]; }
  double v_fov() const { return vFov; }
  double aspect_ratio() const { return ar; }
  double near_clip() const { return nClip; }
  double far_clip() const { return fClip; }


  double nClip, fClip;

//  Mouse Control
  enum e_mouse_button {
    LEFT   = MOUSE_LEFT,
    RIGHT  = MOUSE_RIGHT,
    MIDDLE = MOUSE_MIDDLE
  };

  bool leftDown;
  bool rightDown;
  bool middleDown;

  float mouseX,mouseY;

  void mouse_released(e_mouse_button b);
  void mouse1_dragged(float x, float y);
  void mouse2_dragged(float x, float y);
  void mouse_moved(float x, float y){
    y = screenH - y;
  }
  
  void mouse_event(int key, int event, unsigned char mods);
  void mouse_pressed(e_mouse_button b);
  void cursor_event(float x, float y);
  void scroll_event(float offset_x, float offset_y);

  Vector3D pos, targetPos;

 private:

  // Computes pos, screenXDir, screenYDir from target, r, phi, theta.
  void compute_position();

  // Field of view aspect ratio, clipping planes.
  double hFov, vFov, ar;
  // Current position and target point (the point the camera is looking at).

  // Orientation relative to target, and min & max distance from the target.
  double phi, theta, r, minR, maxR;

  // camera-to-world rotation matrix (note: also need to translate a
  // camera-space point by 'pos' to perform a full camera-to-world
  // transform)
  Matrix3x3 c2w;

  // Info about screen to render to; it corresponds to the camera's full field
  // of view at some distance.
  size_t screenW, screenH;
  double screenDist;
};

class SimpleView{
public:
    SimpleView(ParticleSet* pSet, Grid* grid, Constants* cons);
    ~SimpleView();

    void init( void );
    void start( void );


    static Camera* camera;

private:

    const std::string vertexShaderSource = STR(
      varying vec3 vertex;
      void main(void)
      {
        vertex = gl_Vertex.xyz;
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
      }
    );

    const std::string fragmentShaderSource = STR(
      varying vec3 vertex;
      void main(void)
      {
        // gl_FragColor = vec4(vertex.z / 20.0 + 0.5,vertex.z / 20.0 + 0.5,vertex.z / 20.0 + 0.5, 1.0);
        gl_FragColor = vec4(vertex.z / 20.0 + 0.5,vertex.z / 20.0 + 0.5,vertex.z / 20.0 + 0.5, 0.4);
      }
    );

    const std::string sphereVertSource = STR(

      attribute vec3 pointinfo;

      void main(void)
      {

        float pointRadius = pointinfo.x;
        float pointScale = pointinfo.y;
        float pointDist = pointinfo.z;

        // vec3 posEye = vec3(gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0)) + vec3(0,0,-2.0f);
        // float dist = length(posEye);

        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
        gl_PointSize =  15.0 * pointRadius * (pointScale / pointDist);
        gl_TexCoord[0] = gl_MultiTexCoord0;
        gl_FrontColor = gl_Color;
      }

    );


    const std::string sphereFragSource = STR(

    void main()
    {
        const vec3 lightDir = vec3(0.577, 0.577, 0.577);

        // calculate normal from texture coordinates
        vec3 N;
        N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
        float mag = dot(N.xy, N.xy);

        if (mag > 1.0) discard;   // kill pixels outside circle

        N.z = sqrt(1.0-mag);

        // calculate lighting
        float diffuse = max(0.0, dot(lightDir, N));

        gl_FragColor = gl_Color * diffuse;
    }

    );

    // const std::string sphereFragSource = STR(
    //   varying vec3 vertex;
    //   void main(void)
    //   {
    //     vec3 N;
    //     N.xy = gl_TexCoord[0].xy;
    //     gl_FragColor = vec4(N.x,N.y,0.0, 1.0);
    //   }
    // );


    GLfloat* vertices_position;

    GLuint shaderProgram,shaderProgram2;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint sphereVertShader;
    GLuint sphereFragShader;

    ParticleSet* pSet;
    Grid* grid;
    Mesh* mesh;
    Vector3D domainExtent;

    GLFWwindow* window;
    int framecount;
    int screenW,screenH;
    void draw_faces();
    void draw_spheres();
    void draw_domainOutline();
    void draw_nodes();
    void draw_nodeVelNext();
    void draw_nodeForce();
    void draw_particleForce();

    void createGLProgram();
    void initGLFW();
    void setGLSettings();
    void setPerspective();
    void timeStep();
    void update( void );
    void update_gl_camera();

    void spitToFile();



    static void mouse_button_callback( GLFWwindow* window, int button, int action, int mods );
    static void cursor_callback( GLFWwindow* window, double xpos, double ypos );
    static void scroll_callback( GLFWwindow* window, double xoffset, double yoffset);
    
    unsigned int currentTimeStep;

    static void err_callback( int error, const char* description );

    size_t buffer_w;
    size_t buffer_h;



};