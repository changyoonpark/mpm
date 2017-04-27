#include "include/particle.h"
#include "include/constants.h"
// #include <omp.h>
#define EPS_D_SMALL (1.E-100)
#define DBL_MAX 1.7976931348623158e+308


void Particle::collectVelocity(velType veltype){

    for(int ii = -2; ii <= 1; ii++){
    for(int jj = -2; jj <= 1; jj++){
    for(int kk = -2; kk <= 1; kk++){
        
        int3 nidx = int3(ii,jj,kk);        
        GridNode* gn = cell->node->neighborNode(nidx);
        if (gn == NULL) continue; 
        if (!gn->isActive) continue;

        if(veltype == NOW){ velNowCollected += gn->vel * gn->W(pos); }
        else if(veltype == NEXT){ velNextCollected += gn->velNext * gn->W(pos); }
    }    
    }
    }
}
// x:4.5,4.5,4.0 v:0,0,-1 m:1.0,0,0
// x:4.5,5.0,4.0 v:0,0,-1 m:1.0,0,0
// x:4.5,5.5,4.0 v:0,0,-1 m:1.0,0,0
// x:5.0,4.5,4.0 v:0,0,-1 m:1.0,0,0
// x:5.0,5.0,4.0 v:0,0,-1 m:1.0,0,0
// x:5.0,5.5,4.0 v:0,0,-1 m:1.0,0,0
// x:5.5,4.5,4.0 v:0,0,-1 m:1.0,0,0
// x:5.5,5.0,4.0 v:0,0,-1 m:1.0,0,0
// x:5.5,5.5,4.0 v:0,0,-1 m:1.0,0,0

void Particle::calculateSignedDistance(){


    double dist;
    signedDist = DBL_MAX;

    bool didContact = false;
    for( auto& tri : consts.mesh->triangles){
        if(tri.getDistance(pos,dist)){
            if(dist < signedDist){
                contactingTriangle = &tri;
                signedDist = dist;      
                didContact = true;
            } 
        } 
    }

    if (!didContact) signedDist = DBL_MAX;

    if(signedDist < 0.) isInsideGeometry = true;
    else               isInsideGeometry = false;

    // for(int ii = -2; ii <= 1; ii++)
    // for(int jj = -2; jj <= 1; jj++)
    // for(int kk = -2; kk <= 1; kk++){
        
    //     int3 nidx = int3(ii,jj,kk);
    //     GridNode* gn = cell->node->neighborNode(nidx);
    //     if (gn == NULL) continue;
    //     if (!gn->isActive) continue;
    //     signedDist += gn->signedDist * gn->W(pos);
    //     gradSignedDist += gn->contactingTriangle->getNormal() * gn->W(pos);
    // }    
    
    // if (signedDist < 0.){
    //     isInsideGeometry = true;
    // } else{
    //     isInsideGeometry = false;
    // }
}

void Particle::calculateDensityAndVolume(){
    density = 0.;
    // std::cout << "pid : " << pID << " at : " << pos << " cell node : ";
    // std::cout << cell->node->idx.nx  << ", " << cell->node->idx.ny << ", " << cell->node->idx.nz << std::endl;
    for(int ii = -2; ii <= 1; ii++){
    for(int jj = -2; jj <= 1; jj++){
    for(int kk = -2; kk <= 1; kk++){
        
        int3 nidx = int3(ii,jj,kk);
        GridNode* gn = cell->node->neighborNode(nidx);
        if (gn == NULL) continue;
        if (!gn->isActive) continue;
        density += gn->mass * gn->W(pos);

    }}}

    density = density / (consts.h * consts.h * consts.h );
    volume = mass / density;
    // printf("Volume = %f, Density = %f\n",volume,density);
}



void Particle::calculateVelocityGradient(){
    gradVelocity = 0. * gradVelocity;
    Vector3D gW;

    //Index through the surrounding nodes.
    for(int ii = -2; ii <= 1; ii++)
    for(int jj = -2; jj <= 1; jj++)
    for(int kk = -2; kk <= 1; kk++){
        int3 nidx = int3(ii,jj,kk);
        GridNode* gn = cell->node->neighborNode(nidx);
        if (gn == NULL) continue;
        if (!gn->isActive) continue;
        gW = gn->gW(pos);
        gradVelocity += Matrix3x3(gn->velNext.x * gW,
                                  gn->velNext.y * gW,
                                  gn->velNext.z * gW);       
    }    

}



Matrix3x3 Particle::calculateAMatrix(Vector3D& delta_u){
    //Calculate delta_F_E
    Matrix3x3 delta_F_E;
    delta_F_E.zero();
    for(int ii = -2; ii <= 1; ii++){
    for(int jj = -2; jj <= 1; jj++){
    for(int kk = -2; kk <= 1; kk++){
        int3 nidx = int3(ii,jj,kk);
        GridNode* gn = cell->node->neighborNode(nidx);
        if (gn == NULL) continue;
        if (!gn->isActive) continue;
        delta_F_E += tensorProduct(delta_u, gn->gW(pos)) * F_E;
    }}}    

    return consts.snowModel->hessianWrtF_doubleContractionDeltaF(J_E,J_P,
                                                                 F_E,
                                                                 R_E,
                                                                 S_E,
                                                                 F_E_inv,
                                                                 delta_F_E);    

}

void Particle::updateVelNext(){
    velNextCollected = Vector3D(0.,0.,0.);
    velNowCollected = Vector3D(0.,0.,0.);

    collectVelocity(NEXT);
    collectVelocity(NOW);

    // std::cout 
    // if (velNowCollected.z < 0){
    // std::cout << "----" << std::endl;
    // std::cout << velNowCollected << std::endl;
    // std::cout << velNextCollected << std::endl;
    // }
    
    velPIC = velNextCollected;
    velFLIP = (vel + velNextCollected - velNowCollected);
    velNext = (1. - consts.snowModel->alpha ) * velPIC + consts.snowModel->alpha * velFLIP;
    // std::cout << velNextCollected << ", " << velNowCollected << std::endl;
    // velNext = velFLIP;
    // velNext = 0.5 * velNowCollected + 0.5 * velNextCollected;
    // std::cout << "********" << std::endl;
    // std::cout << "velPIC : " << velPIC << std::endl;
    // std::cout << "velFLIP : " << velFLIP << std::endl;
    // std::cout << "vel : " << vel << std::endl;
    // std::cout << "velNext : " << velNext << std::endl;

}

void Particle::calculateGeometryInteraction(){

	if (isInsideGeometry){	
		Vector3D relVel = velNext - contactingTriangle->vel;
		double vn = dot(relVel, contactingTriangle->getNormal());
		//material and boundary is coming together

		if ( vn  <= 0. ){
			Vector3D vTan = relVel - vn * contactingTriangle->getNormal(); 
			double vTanLength = vTan.norm();

			// if (vel.norm() > 0.01) std::cout << vTanLength << std::endl;
                relVel = Vector3D(0.,0.,0.);
			// if(vTanLength <  - consts.snowModel->muStick * vn){
			// 	relVel = Vector3D(0.,0.,0.);
			// } else{
			// 	relVel = vTan + consts.snowModel->muStick * vn * vTan / vTanLength;
			// }

			velNext = relVel + contactingTriangle->vel;
		}
                	
    }    

}


void Particle::updateDeformationGradient(){
    // Simple update.
    // D = gradVelocity * F;
    // F = F + consts.dt * D;
    // J = F.det();
    // J_E = J;
    // F_E = F;
    // R_E = R_E.identity();
    // F_P = F_P.identity();
    // J_P = 1.;

    // Plastic Update.
    F_E = ( F_E.identity() + consts.dt * gradVelocity ) * F_E;
    F = F_E * F_P;

    F_E.singularValueDecompose(U,SIGMA,V);
    R_E = U * V.T();
    S_E = R_E.inv() * F_E;

    SIGMA.clamp(1. - consts.snowModel->theta_compression,
                1. + consts.snowModel->theta_stretch     );

    F_E = U * SIGMA * V.T();
    F_P = F_E.inv() * F;

    J_E = F_E.det();
    J_P = F_P.det();
    J = J_E * J_P;

    F_E_inv = F_E.inv();

}

void Particle::initParticle(){
    velNext = Vector3D(0.,0.,0.);
    velFLIP = Vector3D(0.,0.,0.);
    velPIC = Vector3D(0.,0.,0.);
    velNowCollected = Vector3D(0.,0.,0.);
    velNextCollected = Vector3D(0.,0.,0.);
}


ParticleSet::ParticleSet(Constants& _consts, InitData& dat)
    : consts ( _consts )
 {
    unsigned int n = 0;
    Vector3D pos,vel;    
    std::cout << "Initializing " << dat.data.size() << " Particles..." << std::endl;    
    for(auto entry : dat.data){
        particleSet.push_back(Particle(n,entry,consts));
        n++;
    }    
}

void ParticleSet::rasterizeParticlesOntoNodes(){
    std::cout << "rasterizing .. " << std::endl;
	#pragma omp parallel for num_threads(THREADCOUNT)
	for (int i = 0; i < particleSet.size(); i++){
        double W;   
        //Index through the surrounding nodes.
        for(int ii = -2; ii <= 1; ii++){
        for(int jj = -2; jj <= 1; jj++){
        for(int kk = -2; kk <= 1; kk++){

            int3 nidx = int3(ii,jj,kk);
            GridNode* gn = particleSet[i].cell->node->neighborNode(nidx);

            if (gn == NULL) continue;
            W = gn->W(particleSet[i].pos);


            #pragma omp atomic
                gn->mass += particleSet[i].mass * W;

            #pragma omp critical (activateNodes)
            {
                gn->_grid->activeNodes[std::make_tuple(gn->idx.nx,gn->idx.ny,gn->idx.nz)] = gn;
                gn->isActive = true;
            }
    

        }}}

    }        

	#pragma omp parallel for num_threads(THREADCOUNT)
	for (int i = 0; i < particleSet.size(); i++){

        double W;
        //Index through the surrounding nodes.
        for(int ii = -2; ii <= 1; ii++){
        for(int jj = -2; jj <= 1; jj++){
        for(int kk = -2; kk <= 1; kk++){

            int3 nidx = int3(ii,jj,kk);
            GridNode* gn = particleSet[i].cell->node->neighborNode(nidx);

            if (gn == NULL) continue;
            if (!gn->isActive) continue;
            W = gn->W(particleSet[i].pos);

            #pragma omp critical (velcollect)
            {
                gn->vel += particleSet[i].vel * particleSet[i].mass * W / (gn->mass + EPS_D_SMALL);
            }   

        }}}

    }        

}

void ParticleSet::updateParticleSignedDistance(){
	#pragma omp parallel for num_threads(THREADCOUNT)
	for (int i = 0; i < particleSet.size(); i++){
        particleSet[i].calculateSignedDistance();    
    }    
}

void ParticleSet::estimateParticleVolume(){
    // std::cout << "size : " << particleSet.size() << std::endl;
	#pragma omp parallel for num_threads(THREADCOUNT)
	for (int i = 0; i < particleSet.size(); i++){
        particleSet[i].calculateDensityAndVolume();    
    }
}

void ParticleSet::calculateParticleVelocityGradient(){
	#pragma omp parallel for num_threads(THREADCOUNT)
	for (int i = 0; i < particleSet.size(); i++){
        particleSet[i].calculateVelocityGradient();    
    }
}

void ParticleSet::updateParticlePosition(){

	#pragma omp parallel for num_threads(THREADCOUNT)
	for (int i = 0; i < particleSet.size(); i++){
        particleSet[i].updateVelNext();    
    }

    updateParticleSignedDistance();
    calculateGeometryInteractions();

	#pragma omp parallel for num_threads(THREADCOUNT)
 	for (int i = 0; i < particleSet.size(); i++){
        particleSet[i].force = particleSet[i].mass * (particleSet[i].velNext - particleSet[i].vel)/consts.dt;      
        particleSet[i].vel = particleSet[i].velNext;
        if (DIMENSION == 2){
            particleSet[i].vel.y = 0;
        }
        particleSet[i].pos += particleSet[i].vel * consts.dt;
        particleSet[i].initParticle();


    }
 
}

void ParticleSet::updateParticleDeformationGradient(){
	#pragma omp parallel for num_threads(THREADCOUNT)    
	for (int i = 0; i < particleSet.size(); i++){        
        particleSet[i].updateDeformationGradient();    
    }    
    
}

void ParticleSet::calculateGeometryInteractions(){
    std::cout << "calculating geometry interactions for the particles" << std::endl;
	#pragma omp parallel for num_threads(THREADCOUNT)    
	for (int i = 0; i < particleSet.size(); i++){        
        particleSet[i].calculateGeometryInteraction();    
    }    
    
}

