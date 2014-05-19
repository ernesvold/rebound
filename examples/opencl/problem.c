/**
 * @file 	problem.c
 * @brief 	Example problem: opencl.
 * @author 	Hanno Rein <hanno@hanno-rein.de>
 * @detail 	A self-gravitating disc is integrated using
 * the OpenCL direct gravity summation module.
 *
 * This is a very simple implementation (see gravity_opencl.c). 
 * Currently it only supports floating point precission. It also
 * transfers the data back and forth from the GPU every timestep.
 * There are considerable improvements to be made. This is just a 
 * proof of concept. Also note that the code required N to be a 
 * multiple of the workgrop size.
 *
 * You can test the performance increase by running:
 * make direct && ./nbody       this will run on the CPU
 * make && ./nbody              this will run on the GPU
 *
 *
 * 
 * @section 	LICENSE
 * Copyright (c) 2014 Hanno Rein
 *
 * This file is part of rebound.
 *
 * rebound is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rebound is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rebound.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "main.h"
#include "particle.h"
#include "boundaries.h"
#include "output.h"
#include "tree.h"
#include "tools.h"

extern int Nmax;

void problem_init(int argc, char* argv[]){
	// Setup constants
	G 		= 1;		
	softening 	= 0.01;		
	dt 		= 3e-3;
	boxsize 	= 2.4;
	root_nx = 1; root_ny = 1; root_nz = 1;
	nghostx = 0; nghosty = 0; nghostz = 0; 		
	init_box();

	// Initial conditions
	struct particle star;
	star.x 		= 0; star.y 	= 0; star.z	= 0;
	star.vx 	= 0; star.vy 	= 0; star.vz 	= 0;
	star.ax 	= 0; star.ay 	= 0; star.az 	= 0;
	star.m 		= 1;
	particles_add(star);
	
	// Setup disk particles
	double disc_mass = 2e-1;
	int _N = 1024*4;
	while(N<_N){
		struct particle pt;
		double a	= tools_powerlaw(boxsize/20.,boxsize/4./1.2,-1.5);
		double phi 	= tools_uniform(0,2.*M_PI);
		pt.x 		= a*cos(phi);
		pt.y 		= a*sin(phi);
		pt.z 		= a*tools_normal(0.001);
		double mu 	= star.m + disc_mass * (pow(a,-3./2.)-pow(boxsize/20.,-3./2.))/(pow(boxsize/4./1.2,-3./2.)-pow(boxsize/20.,-3./2.));
		double vkep 	= sqrt(G*mu/a);
		pt.vx 		=  vkep * sin(phi);
		pt.vy 		= -vkep * cos(phi);
		pt.vz 		= 0;
		pt.ax 		= 0;
		pt.ay 		= 0;
		pt.az 		= 0;
		pt.m 		= disc_mass/(double)_N;
		particles_add(pt);
	}
}

void problem_inloop(){
}

void problem_output(){
	if (output_check(10.0*dt)) output_timing();
}

void problem_finish(){
}
