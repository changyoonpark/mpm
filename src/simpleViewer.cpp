#include "include/simpleViewer.h"
// #include <omp.h>

Camera* SimpleView::camera;

void Camera::configure(size_t screenW, size_t screenH) {

  this->screenW = screenW;
  this->screenH = screenH;

  hFov = 3.141592 / 3.3;
  vFov = 3.141592 / 3.3;

  nClip = 0.1;
  fClip = 2000.0f;

  double ar1 = tan(CGL::radians(hFov) / 2) / tan(CGL::radians(vFov) / 2);
  ar = static_cast<double>(screenW) / screenH;
  if (ar1 < ar) {
    // hFov is too small
    hFov = 2 * CGL::degrees(atan(tan(CGL::radians(vFov) / 2) * ar));
  } else if (ar1 > ar) {
    // vFov is too small
    vFov = 2 * CGL::degrees(atan(tan(CGL::radians(hFov) / 2) / ar));
  
  }
  screenDist = ((double) screenH) / (2.0 * tan(CGL::radians(vFov) / 2));
}


void Camera::place(const Vector3D& targetPos, const double phi,
                   const double theta, const double r, const double minR,
                   const double maxR) {
  double r_ = std::min(std::max(r, minR), maxR);
  double phi_ = (sin(phi) == 0) ? (phi + EPS_F) : phi;
  this->targetPos = targetPos;
  this->phi = phi_;
  this->theta = theta;
  this->r = r_;
  this->minR = minR;
  this->maxR = maxR;
  compute_position();
}


void Camera::move_by(const double dx, const double dy, const double d) {
  const double scaleFactor = d / screenDist;
  const Vector3D& displacement =
    c2w[0] * (dx * scaleFactor) + c2w[1] * (dy * scaleFactor);
  pos += displacement;
  targetPos += displacement;
}

void Camera::move_forward(const double dist) {
  double newR = std::min(std::max(r - dist, minR), maxR);
  pos = targetPos + ((pos - targetPos) * (newR / r));
  r = newR;
}

void Camera::rotate_by(const double dPhi, const double dTheta) {
  phi = CGL::clamp(phi + dPhi, 0.0, (double) PI);
  phi += dPhi;
  theta += dTheta;
  compute_position();
}

void Camera::compute_position() {
  double sinPhi = sin(phi);
  if (sinPhi == 0) {
    phi += EPS_F;
    sinPhi = sin(phi);
  }
  // const Vector3D dirToCamera(r * sinPhi * sin(theta),
  //                            r * cos(phi),
  //                            r * sinPhi * cos(theta));
  const Vector3D dirToCamera(r * sinPhi * sin(theta),
                             r * sinPhi * cos(theta),
                             r * cos(phi));

  pos = targetPos + dirToCamera;
  // Vector3D upVec(0, sinPhi > 0 ? -1 : 1, 0);
  Vector3D upVec (0,0,1.0);
  Vector3D screenXDir = cross(upVec, dirToCamera);
  screenXDir.normalize();
  Vector3D screenYDir = cross(dirToCamera, screenXDir);
  screenYDir.normalize();

  // std::cout << "cam setting" << std::endl;
  // std::cout << theta << std::endl;
  // std::cout << phi << std::endl;
  // std::cout << u << std::endl;

  c2w[0] = screenXDir;
  c2w[1] = screenYDir;
  c2w[2] = dirToCamera.unit();   // camera's view direction is the
                                 // opposite of of dirToCamera, so
                                 // directly using dirToCamera as
                                 // column 2 of the matrix takes [0 0 -1]
                                 // to the world space view direction
}



void Camera::scroll_event(float offset_x, float offset_y) {

  // update_style();
  move_forward(-offset_y * viewDist / 10.0);
}

void Camera::mouse_event(int key, int event, unsigned char mods){
  switch(event) {
    case EVENT_PRESS:
      switch(key) {
        case MOUSE_LEFT:
          mouse_pressed(LEFT);
          break;
        case MOUSE_RIGHT:
          mouse_pressed(RIGHT);
          break;
        case MOUSE_MIDDLE:
          mouse_pressed(MIDDLE);
          break;
      }
      break;
    case EVENT_RELEASE:
      switch(key) {
        case MOUSE_LEFT:
          mouse_released(LEFT);
          break;
        case MOUSE_RIGHT:
          mouse_released(RIGHT);
          break;
        case MOUSE_MIDDLE:
          mouse_released(MIDDLE);
          break;
      }
      break;
  }
} 




void Camera::mouse_pressed(e_mouse_button b) {
  switch (b) {
    case LEFT:
      leftDown = true;
      break;
    case RIGHT:
      rightDown = true;
      break;
    case MIDDLE:
      middleDown = true;
      break;
  }
}

void Camera::mouse_released(e_mouse_button b) {
  switch (b) {
    case LEFT:
      leftDown = false;
      break;
    case RIGHT:
      rightDown = false;
      break;
    case MIDDLE:
      middleDown = false;
      break;
  }
}

/*
  When in edit mode and there is a selection, move the selection.
  When in visualization mode, rotate.
*/
void Camera::mouse1_dragged(float x, float y) {
  float dx = (x - mouseX);
  float dy = (y - mouseY);
  
//   rotate_by(-dy * (PI / screenH), -dx * (PI / screenW));
  rotate_by(-dy * (PI / screenH), 0.);

}

/*
  When the mouse is dragged with the right button held down, translate.
*/
void Camera::mouse2_dragged(float x, float y) {
//   float dx = (x - mouseX);
//   float dy = (y - mouseY);

//   move_by(-dx, dy, viewDist);
  float dx = (x - mouseX);
  float dy = (y - mouseY);
  
//   rotate_by(-dy * (PI / screenH), -dx * (PI / screenW));
  rotate_by(0. , -dx * (PI / screenW));
}

void Camera::cursor_event(float x, float y) {

  if (leftDown && !middleDown && !rightDown) {
    mouse1_dragged(x, y);
  } else if (!leftDown && !middleDown && rightDown) {
    mouse2_dragged(x, y);
  } else if (!leftDown && !middleDown && !rightDown) {
    mouse_moved(x, y);
  }

  mouseX = x;
  mouseY = y;
}

SimpleView::SimpleView(ParticleSet* _pSet, Grid* _grid, Constants* _cons){
    camera = new Camera;
    pSet = _pSet;
    grid = _grid;
    mesh = _cons->mesh;
    domainExtent = _cons->domainExtent;
}
                        

SimpleView::~SimpleView(){
    glfwDestroyWindow(window);
    glfwTerminate();
}

void SimpleView::err_callback( int error, const char* description ) {
    std::cout << "GLFW ERROR" << std::endl;
}

void SimpleView::start(){
  #if WITH_GUI == 1
    while( !glfwWindowShouldClose( window ) ){
      update();
    } 
  #else
    while(1){
      update();
    }
  #endif  
  // update();
}

void SimpleView::setPerspective(){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(camera->v_fov(),
                 camera->aspect_ratio(),
                 camera->near_clip(),
                 camera->far_clip());
}

void SimpleView::update_gl_camera() {

  // Call resize() every time we draw, since it doesn't seem
  // to get called by the Viewer upon initial window creation
  // (this should probably be fixed!).
  // GLint view[4];
  // glGetIntegerv(GL_VIEWPORT, view);
//   if (view[2] != screenW || view[3] != screenH) {
//     resize(view[2], view[3]);
//   }

  // Control the camera to look at the mesh.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  const Vector3D& c = camera->position();
  const Vector3D& r = camera->view_point();
  const Vector3D& u = camera->up_dir();

  // gluLookAt(0.0, 0.0, 0.145,
  //           0.0, 0.0, 1.0,
  //           0.0, 1.0, 0.0);

  gluLookAt(c.x, c.y, c.z,
            r.x, r.y, r.z,
            u.x, u.y, u.z);
  // gluLookAt(c.x, c.y, c.z,
  //           r.x, r.y, r.z,
  //           u.x, u.y, u.z);

}

void SimpleView::draw_nodes(){
    glUseProgram(shaderProgram);    
    for (auto& node : grid->nodes){      
      double dens = std::min(node->mass,1.0);
      glBegin(GL_POINTS);    
      glPointSize(10.0);
      // glColor3f(dens,dens,dens);
      glColor3f(1.0,1.0,1.0);
      glVertex3f(node->x.x,node->x.y,node->x.z);
      glEnd();
    }
}

void SimpleView::draw_spheres(){

    glUseProgram(shaderProgram2);
    glEnable(GL_POINT_SPRITE_ARB);
    glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);;

    GLint loc1 = 1;
    glBindAttribLocation(shaderProgram2, loc1, "pointinfo");
    for (auto& particle : pSet->particleSet){

        // glBegin(GL_LINES);      
        glBegin(GL_POINTS);        
        glVertexAttrib3f( loc1,
                          100.0,
                          2. * camera->viewDist * pSet->consts.h,
                          (GLfloat)(particle.pos - camera->pos).norm() );
        glColor3f(1, 1, 1);
        glVertex3f( particle.pos.x, particle.pos.y, particle.pos.z);
        glEnd();
    }
    
    glUseProgram(0);
    glDisable(GL_POINT_SPRITE_ARB);        
}

void SimpleView::draw_nodeForce(){
    glUseProgram(shaderProgram);
    glEnable(GL_DEPTH_TEST);  
    for (auto& node : grid->nodes){ 
        glBegin(GL_LINE_STRIP);           
        glColor3f(1.0,0.0,0.0);  
        glVertex3f( node->x.x, node->x.y, node->x.z);
        glVertex3f( node->x.x,
                    node->x.y,
                    node->x.z + node->mass);
        // glVertex3f( node->x.x + node->force.x * 0.2,
        //             node->x.y + node->force.y * 0.2,
        //             node->x.z + node->force.z * 0.2);
        glEnd();
    }  
}

void SimpleView::draw_nodeVelNext(){
    glUseProgram(shaderProgram);
    glEnable(GL_DEPTH_TEST);  
    for (auto& node : grid->nodes){ 
        glBegin(GL_LINE_STRIP);           
        glColor3f(1.0,0.0,0.0);  
        glVertex3f( node->x.x, node->x.y, node->x.z);
        glVertex3f( node->x.x + node->velNext.x,
                    node->x.y + node->velNext.y,
                    node->x.z + node->velNext.z);
        glEnd();
    }  
}

void SimpleView::draw_particleForce(){

    glUseProgram(shaderProgram);
    glEnable(GL_DEPTH_TEST);  

    for (auto& p : pSet->particleSet){ 
    
        glBegin(GL_LINE_STRIP);           
        glColor3f(1.0,0.0,0.0);  
        glVertex3f( p.pos.x, p.pos.y, p.pos.z);
        glVertex3f( p.pos.x + p.force.x * 0.1,
                    p.pos.y + p.force.y * 0.1,
                    p.pos.z + p.force.z * 0.1);
        glEnd();

    }  
}


void SimpleView::draw_faces(){
    glUseProgram(shaderProgram);
    glEnable(GL_DEPTH_TEST);  
    for (auto& tri : mesh->triangles){

        // glBegin(GL_LINES);      
        
        // glBegin(GL_TRIANGLES);        
        glBegin(GL_LINE_STRIP);             
        glVertex3f( tri.p1.x, tri.p1.y, tri.p1.z);
        glVertex3f( tri.p2.x, tri.p2.y, tri.p2.z);
        glVertex3f( tri.p3.x, tri.p3.y, tri.p3.z);
        glVertex3f( tri.p1.x, tri.p1.y, tri.p1.z);
        glEnd();
    }
    

}

void SimpleView::timeStep(){


    //Map particle properties onto the grid.
    grid->hashParticles();
    // pSet->rasterizeParticlesOntoNodes();
    grid->rasterizeNodes();
    
    // double totmass = 0., totmass2 = 0.;
    // for(auto& node : grid->nodes){
    //   totmass += node->mass;
    // }
    // std::cout << "total mass : " << totmass << std::endl;

    mesh->updatePosition(pSet->consts.dt);
    grid->calculateSignedDistance(); 

    if (currentTimeStep == 0 ){
        //Initialize
        pSet->estimateParticleVolume();       
        // double totmass2 = 0.;
        // for(auto& p : pSet->particleSet){
        //   totmass2 += p.volume * p.density;
        // }
        // std::cout << "total mass : " << totmass2 << std::endl;

    }

    //Grid Velocity update.
    grid->calculateNodalForcesAndUpdateVelocities();
    grid->calculateGeometryInteractions();
    // grid->solveForVelNextAndUpdateVelocities();

    
    pSet->calculateParticleVelocityGradient();

    pSet->updateParticleDeformationGradient();
    
    pSet->updateParticlePosition();

    if(currentTimeStep % 1 == 0) spitToFile();

    std::cout << "---------------End of Timestep : " << currentTimeStep << "-----------------------" << std::endl;
    currentTimeStep ++;

}

void SimpleView::spitToFile(){
  double h3 = pSet->consts.h * pSet->consts.h * pSet->consts.h;

  std::cout << "outputing data." << std::endl;


  #pragma omp parallel for num_threads(THREADCOUNT)
  for(int t = 0; t < THREADCOUNT; t ++){
    // int nodesPerThread = grid->activeNodes.size() / THREADCOUNT;
    int nodesPerThread = grid->nodes.size() / THREADCOUNT;
    // int tid = omp_get_thread_num();
    int tid = t;
    int startIdx = tid * nodesPerThread;
    int endIdx = (tid + 1) * nodesPerThread;

    if (tid == THREADCOUNT - 1){
      nodesPerThread = grid->nodes.size() - nodesPerThread * (THREADCOUNT - 1);      
      endIdx = grid->nodes.size();
    } 
    
    std::string fileName = "./outputs/grid_t_";

    fileName += std::to_string(currentTimeStep);
    fileName += "_part_";
    fileName += std::to_string(tid);
    fileName += ".txt";

    std::ofstream timeStepData;
    timeStepData.open(fileName, std::ofstream::out | std::ofstream::trunc);

  
    if(!timeStepData) std::cout << "file cant be opened" << std::endl;
    for(int i=startIdx;i<endIdx;i++){
      // auto dataIt = grid->nodes.begin();
      // std::advance(dataIt,i);
      // GridNode* gn = dataIt->second;
      GridNode* gn = grid->nodes[i];
      timeStepData << gn->x.x << "," <<  gn->x.y << "," << gn->x.z << " ";
      timeStepData << gn->mass / h3 << "\n";
      timeStepData.flush();
    }
    timeStepData.close();
  }



  #pragma omp parallel for num_threads(THREADCOUNT)
  for(int t = 0; t < THREADCOUNT; t ++){
    int particlesPerThread = pSet->particleSet.size() / THREADCOUNT;
    // int tid = omp_get_thread_num();
    int tid = t;
    int startIdx = tid * particlesPerThread;
    int endIdx = (tid + 1) * particlesPerThread;

    if (tid == THREADCOUNT - 1){
      particlesPerThread = pSet->particleSet.size() - particlesPerThread * (THREADCOUNT - 1);      
      endIdx = pSet->particleSet.size();
    } 
    
    std::string fileName = "./outputs/particles_t_";
    fileName += std::to_string(currentTimeStep);
    fileName += "_part_";
    fileName += std::to_string(tid);
    fileName += ".txt";

    std::ofstream timeStepData;
    timeStepData.open(fileName, std::ofstream::out | std::ofstream::trunc);

    if(!timeStepData) std::cout << "file cant be opened" << std::endl;
    for(int i=startIdx;i<endIdx;i++){
      Particle p = pSet->particleSet[i];
      timeStepData << p.pos.x << "," <<  p.pos.y << "," << p.pos.z << "\n";
      timeStepData.flush();
    }
    timeStepData.close();
  }

}

void SimpleView::createGLProgram(){
  shaderProgram = glCreateProgram();
  shaderProgram2 = glCreateProgram();

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  sphereVertShader = glCreateShader(GL_VERTEX_SHADER);
  sphereFragShader = glCreateShader(GL_FRAGMENT_SHADER);
  
  const char* c_str1 = vertexShaderSource.c_str();
  glShaderSource(vertexShader, 1, &c_str1 , NULL);
  glCompileShader(vertexShader);

  const char* c_str2 = fragmentShaderSource.c_str();
  glShaderSource(fragmentShader, 1, &c_str2 , NULL);
  glCompileShader(fragmentShader);

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);


  const char* c_str3 = sphereVertSource.c_str();
  glShaderSource(sphereVertShader, 1, &c_str3 , NULL);
  glCompileShader(sphereVertShader);

  const char* c_str4 = sphereFragSource.c_str();
  glShaderSource(sphereFragShader, 1, &c_str4 , NULL);
  glCompileShader(sphereFragShader);

  glAttachShader(shaderProgram2, sphereVertShader);
  glAttachShader(shaderProgram2, sphereFragShader);

  glLinkProgram(shaderProgram);
  glLinkProgram(shaderProgram2);

}

void SimpleView::initGLFW(){

  glfwSetErrorCallback( err_callback );
  if( !glfwInit() ) {
    std::cout << "GLFW INIT FALIED." << std::endl;
    exit( 1 );
  }

  screenW = 600;
  screenH = 600;
  std::string title = "MPM SOLVER";

    window = glfwCreateWindow( screenW, screenH, title.c_str(), NULL, NULL );
    if (!window) {
      std::cout << "WINDOW CREATE FAILED" << std::endl;
      glfwTerminate();
      exit( 1 );
    }

    glfwMakeContextCurrent( window );
    glfwSwapInterval(1);
    
    // initialize glew
    if (glewInit() != GLEW_OK) {
      std::cout << "GLEW INIT FAILED" << std::endl;
      glfwTerminate();
      exit( 1 );
    }

}

void SimpleView::setGLSettings(){

  const Vector3D lpos = Vector3D(domainExtent.x/2.,domainExtent.y/2.,domainExtent.z * 0.95);

  glEnable(GL_DEPTH_TEST);

  // glDepthMask(GL_TRUE);  
  // glDepthFunc(GL_GREATER);  

  GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_ambient[] = {.2, .2, .2, 1.0};
  GLfloat light_position[] = { (float) lpos.x, (float) lpos.y, (float) lpos.z, 0.0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable( GL_PROGRAM_POINT_SIZE );

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_LINE_SMOOTH );
  glEnable( GL_POLYGON_SMOOTH );
  glEnable(GL_POINT_SMOOTH);
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
  glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
  // wheel event callbacks
  // glfwSetScrollCallback(window, scroll_callback);  
  
  // mouse button callbacks
  glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback( window, cursor_callback );
  glfwSetScrollCallback(window, scroll_callback);  

}

void SimpleView::draw_domainOutline(){
    // glBegin(GL_LINE_STRIP);        
    //     glVertex3f( 0.0, 0.0, 0.0 );
    //     glVertex3f( domainExtent.x, 0.0, 0.0 );
    //     glVertex3f( domainExtent.x, domainExtent.y, 0.0 );
    //     glVertex3f( 0.0, domainExtent.y, 0.0 );
    //     glVertex3f( 0.0, 0.0, 0.0 );
    // glEnd();    

    // glBegin(GL_LINE_STRIP);        
    //     glVertex3f( 0.0, 0.0, domainExtent.z );
    //     glVertex3f( domainExtent.x, 0.0, domainExtent.z );
    //     glVertex3f( domainExtent.x, domainExtent.y, domainExtent.z );
    //     glVertex3f( 0.0, domainExtent.y, domainExtent.z );
    //     glVertex3f( 0.0, 0.0, domainExtent.z );
    // glEnd();    

    // glBegin(GL_LINES);        
    //     glVertex3f( 0.0, 0.0, 0.0 );
    //     glVertex3f( 0.0, 0.0, domainExtent.z );
    // glEnd();
    // glBegin(GL_LINES);        
    //     glVertex3f( domainExtent.x, 0.0, 0.0 );
    //     glVertex3f( domainExtent.x, 0.0, domainExtent.z );
    // glEnd();
    // glBegin(GL_LINES);         
    //     glVertex3f( domainExtent.x, domainExtent.y, 0.0 );
    //     glVertex3f( domainExtent.x, domainExtent.y, domainExtent.z );
    // glEnd();
    // glBegin(GL_LINES);        
    //     glVertex3f( 0.0, domainExtent.y, 0.0 );
    //     glVertex3f( 0.0, domainExtent.y, domainExtent.z );
    // glEnd();    
}

void SimpleView::update(){
    #if WITH_GUI == 1
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      update_gl_camera();

      glEnable(GL_LIGHTING);
      draw_domainOutline();
      draw_faces();
      draw_spheres();
      draw_nodes();
      // draw_nodeForce();
      glDisable(GL_LIGHTING);
    #endif

    timeStep();

    #if WITH_GUI == 1
      glfwSwapBuffers(window); 
      glfwPollEvents();
    #endif    
}



void SimpleView::init(){

  currentTimeStep = 0;

  // resize components to current window size, get DPI
  //   glfwGetFramebufferSize(window, (int*) &buffer_w, (int*) &buffer_h );

  // resize elements to current size
  //   resize_callback(window, buffer_w, buffer_h);  
  #if WITH_GUI == 1
    initGLFW();
    setGLSettings();
    createGLProgram();

    camera->configure(screenW, screenH);
    camera->viewDist = domainExtent.norm() * 10.0;

// void Camera::place(const Vector3D& targetPos, const double phi,
//                    const double theta, const double r, const double minR,
//                    const double maxR) {


    camera->place(domainExtent / 2.,
                  3.14/2.,0.,
                 // -2.0,
                 // 1.0,
                //  1.0,
                 camera->viewDist * 5.0,
                 camera->nClip,
                 camera->fClip
    );
    // camera.place(domainExtent / 2.,
    //              0.,
    //              0.,
    //              domainExtent.norm() * 10,
    //              domainExtent.norm() * 0.1,
    //              domainExtent.norm() * 20.0
    //   );
    setPerspective();
  #endif

}


void SimpleView::mouse_button_callback( GLFWwindow* window, int button, int action, int mods ) {
    camera->mouse_event( button, action, mods );
    // std::cout << "Event!" << std::endl;
}

void SimpleView::cursor_callback( GLFWwindow* window, double xpos, double ypos ){
    float cursor_x = xpos;
    float cursor_y = ypos;
    camera->cursor_event(cursor_x, cursor_y);
}

void SimpleView::scroll_callback( GLFWwindow* window, double xoffset, double yoffset) {

  camera->scroll_event(xoffset, yoffset);

}
