#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

typedef struct Body {
    double x, y, z;
    double mass;
    double fx, fy, fz;
} Body;

typedef struct QuadTree {
    double x, y, size;
    double mass;
    double cx, cy;
    struct QuadTree* children[4];
    Body* bodies;
    int body_count;
    int body_capacity;
} QuadTree;

QuadTree* create_quadtree(double x, double y, double size) {
    QuadTree* tree = malloc(sizeof(QuadTree));
    tree->x = x;
    tree->y = y;
    tree->size = size;
    tree->mass = 0;
    tree->cx = 0;
    tree->cy = 0;
    for (int i = 0; i < 4; i++) tree->children[i] = NULL;
    
    tree->body_count = 0;
    tree->body_capacity = 4;
    tree->bodies = malloc(tree->body_capacity * sizeof(Body));
    
    return tree;
}

int get_quadrant(QuadTree* tree, Body* body) {
    if (body->x < tree->x + tree->size / 2) {
        if (body->y < tree->y + tree->size / 2) return 0; // Top-left
        else return 2; // Bottom-left
    } else {
        if (body->y < tree->y + tree->size / 2) return 1; // Top-right
        else return 3; // Bottom-right
    }
}

void subdivide(QuadTree* tree) {
    double half_size = tree->size / 2;
    tree->children[0] = create_quadtree(tree->x, tree->y, half_size);
    tree->children[1] = create_quadtree(tree->x + half_size, tree->y, half_size);
    tree->children[2] = create_quadtree(tree->x, tree->y + half_size, half_size);
    tree->children[3] = create_quadtree(tree->x + half_size, tree->y + half_size, half_size);
    
    printf("Subdivided quadtree region: (%.2f, %.2f) size: %.2f\n", 
           tree->x, tree->y, tree->size);
}

void insert_body(QuadTree* tree, Body* body) {
    if (tree->body_count < 1) {
        if (tree->body_count >= tree->body_capacity) {
            tree->body_capacity *= 2;
            tree->bodies = realloc(tree->bodies, tree->body_capacity * sizeof(Body));
        }
        tree->bodies[tree->body_count++] = *body;
        tree->mass += body->mass;
        tree->cx = body->x;
        tree->cy = body->y;
        
        printf("Inserted body at (%.2f, %.2f) with mass %.2f\n", 
               body->x, body->y, body->mass);
    } else {
        if (tree->children[0] == NULL) {
            subdivide(tree);
        }
        int quadrant = get_quadrant(tree, body);
        insert_body(tree->children[quadrant], body);
    }
}

void compute_forces(QuadTree* tree, Body* body, double theta) {
    double dx = body->x - tree->cx;
    double dy = body->y - tree->cy;
    double dist = sqrt(dx * dx + dy * dy);
    
    if (tree->size / dist < theta || tree->children[0] == NULL) {
        double force = (tree->mass * body->mass) / (dist * dist);
        body->fx += force * (dx / dist);
        body->fy += force * (dy / dist);
        
        printf("Computing force for body at (%.2f, %.2f): fx = %.4f, fy = %.4f\n", 
               body->x, body->y, body->fx, body->fy);
    } else {
        for (int i = 0; i < 4; i++) {
            if (tree->children[i] != NULL) {
                compute_forces(tree->children[i], body, theta);
            }
        }
    }
}

int main() {
    // Seed random number generator for consistent results
    srand(42);

    int N = 1000; // Number of bodies
    Body* bodies = malloc(N * sizeof(Body));

    printf("Initializing %d bodies\n", N);

    // Initialize bodies with more detailed output
    for (int i = 0; i < N; i++) {
        bodies[i].x = rand() % 1000;
        bodies[i].y = rand() % 1000;
        bodies[i].z = rand() % 1000;
        bodies[i].mass = rand() % 1000 + 1;
        bodies[i].fx = 0;
        bodies[i].fy = 0;
        bodies[i].fz = 0;
        
        // Occasional detailed body initialization print
        if (i % 100 == 0) {
            printf("Body %d: pos(%.2f, %.2f, %.2f) mass: %.2f\n", 
                   i, bodies[i].x, bodies[i].y, bodies[i].z, bodies[i].mass);
        }
    }

    printf("Creating quadtree...\n");

    // Create quadtree
    QuadTree* root = create_quadtree(0, 0, 1000);
    for (int i = 0; i < N; i++) {
        insert_body(root, &bodies[i]);
    }

    printf("Computing forces for all bodies...\n");

    // Compute forces
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        compute_forces(root, &bodies[i], 0.5);
    }

    // Print forces for multiple bodies
    printf("\nForce details for first few bodies:\n");
    for (int i = 0; i < 5; i++) {
        printf("Body %d: Force(fx = %f, fy = %f, fz = %f)\n", 
               i, bodies[i].fx, bodies[i].fy, bodies[i].fz);
    }

    // Free memory (simplified, would need more comprehensive cleanup)
    free(bodies);
    free(root);

    return 0;
}