#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

typedef struct {
    double x, y, z;     // Position
    double vx, vy, vz;  // Velocity
    double mass;
} Body;

void compute_forces(Body *bodies, int N) {
    double G = 6.67430e-11; // Gravitational constant
    
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        double fx = 0, fy = 0, fz = 0;
        for (int j = 0; j < N; j++) {
            if (i != j) {
                double dx = bodies[j].x - bodies[i].x;
                double dy = bodies[j].y - bodies[i].y;
                double dz = bodies[j].z - bodies[i].z;
                double distance = sqrt(dx * dx + dy * dy + dz * dz);
                double force = (G * bodies[i].mass * bodies[j].mass) / (distance * distance);
                fx += force * dx / distance;
                fy += force * dy / distance;
                fz += force * dz / distance;
            }
        }
        bodies[i].vx += fx / bodies[i].mass;
        bodies[i].vy += fy / bodies[i].mass;
        bodies[i].vz += fz / bodies[i].mass;
    }
}

void update_positions(Body *bodies, int N, double dt) {
    for (int i = 0; i < N; i++) {
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt;
        bodies[i].z += bodies[i].vz * dt;
    }
}

int main() {
    // Seed random number generator
    srand(time(NULL));

    int N = 1000; // Number of bodies
    double dt = 1e-3; // Time step
    
    // Allocate memory dynamically
    Body *bodies = malloc(N * sizeof(Body));
    if (bodies == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Initialize bodies 
    for (int i = 0; i < N; i++) {
        bodies[i].x = rand() % 1000;
        bodies[i].y = rand() % 1000;
        bodies[i].z = rand() % 1000;
        bodies[i].vx = rand() % 100;
        bodies[i].vy = rand() % 100;
        bodies[i].vz = rand() % 100;
        bodies[i].mass = rand() % 1000 + 1;
    }

    // Simulate N-body system
    for (int t = 0; t < 1000; t++) {
        compute_forces(bodies, N);
        update_positions(bodies, N, dt);
    }

    // Print information about first 10 and last 10 bodies
    printf("First 10 bodies after simulation:\n");
    for (int i = 0; i < 10; i++) {
        printf("Body %d: Pos(%.2f, %.2f, %.2f) Vel(%.2f, %.2f, %.2f) Mass: %.2f\n", 
               i, 
               bodies[i].x, bodies[i].y, bodies[i].z,
               bodies[i].vx, bodies[i].vy, bodies[i].vz,
               bodies[i].mass);
    }

    printf("\nLast 10 bodies after simulation:\n");
    for (int i = N-10; i < N; i++) {
        printf("Body %d: Pos(%.2f, %.2f, %.2f) Vel(%.2f, %.2f, %.2f) Mass: %.2f\n", 
               i, 
               bodies[i].x, bodies[i].y, bodies[i].z,
               bodies[i].vx, bodies[i].vy, bodies[i].vz,
               bodies[i].mass);
    }

    // Free allocated memory
    free(bodies);
    return 0;
}