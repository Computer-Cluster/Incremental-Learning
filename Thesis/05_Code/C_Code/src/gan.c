#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define INPUT_SIZE 10
#define HIDDEN_SIZE 20
#define OUTPUT_SIZE 2
#define SAMPLES 1000
#define EPOCHS 10000
#define LEARNING_RATE 0.01

double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double sigmoid_derivative(double x) {
    return x * (1.0 - x);
}

typedef struct {
    double weights_ih[INPUT_SIZE][HIDDEN_SIZE];
    double weights_ho[HIDDEN_SIZE][OUTPUT_SIZE];
    double bias_h[HIDDEN_SIZE];
    double bias_o[OUTPUT_SIZE];
} NeuralNetwork;

void initialize_network(NeuralNetwork *nn) {
    srand(time(NULL));
    
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            nn->weights_ih[i][j] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        }
    }
    
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            nn->weights_ho[i][j] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        }
    }
    
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        nn->bias_h[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
    }
    
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        nn->bias_o[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
    }
}

void feedforward(NeuralNetwork *nn, double *input, double *output) {
    double hidden[HIDDEN_SIZE] = {0};

    for (int j = 0; j < HIDDEN_SIZE; j++) {
        for (int i = 0; i < INPUT_SIZE; i++) {
            hidden[j] += input[i] * nn->weights_ih[i][j];
        }
        hidden[j] += nn->bias_h[j];
        hidden[j] = sigmoid(hidden[j]);
    }
    
    for (int k = 0; k < OUTPUT_SIZE; k++) {
        output[k] = 0;
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            output[k] += hidden[j] * nn->weights_ho[j][k];
        }
        output[k] += nn->bias_o[k];
        output[k] = sigmoid(output[k]);
    }
}

void train_generator(NeuralNetwork *generator, NeuralNetwork *discriminator) {
    double noise[INPUT_SIZE];
    double generated_data[OUTPUT_SIZE];
    double discriminator_output;
    double error;
    double d_output;

    for (int i = 0; i < INPUT_SIZE; i++) {
        noise[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
    }
    
    feedforward(generator, noise, generated_data);
    
    feedforward(discriminator, generated_data, &discriminator_output);
    
    error = 1.0 - discriminator_output;
    
    d_output = error * sigmoid_derivative(discriminator_output);
    
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            generator->weights_ih[i][j] += LEARNING_RATE * d_output * noise[i];
        }
    }
}

void train_discriminator(NeuralNetwork *discriminator, double *real_data, NeuralNetwork *generator) {
    double noise[INPUT_SIZE];
    double generated_data[OUTPUT_SIZE];
    double discriminator_output;
    double error;
    
    feedforward(discriminator, real_data, &discriminator_output);
    error = 1.0 - discriminator_output; 
    
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            discriminator->weights_ih[i][j] += LEARNING_RATE * error * real_data[i];
        }
    }
    
    for (int i = 0; i < INPUT_SIZE; i++) {
        noise[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
    }
    feedforward(generator, noise, generated_data);
    feedforward(discriminator, generated_data, &discriminator_output);
    error = 0.0 - discriminator_output; // Queremos que salga 0 para datos falsos
    
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            discriminator->weights_ih[i][j] += LEARNING_RATE * error * generated_data[i];
        }
    }
}

int main() {
    NeuralNetwork generator, discriminator;
    
    initialize_network(&generator);
    initialize_network(&discriminator);
    
    double real_data[SAMPLES][OUTPUT_SIZE];
    for (int i = 0; i < SAMPLES; i++) {
        real_data[i][0] = sin(i * 0.1);
        real_data[i][1] = cos(i * 0.1);
    }
    
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        for (int i = 0; i < SAMPLES; i++) {
            train_discriminator(&discriminator, real_data[i], &generator);
        }
        
        for (int i = 0; i < SAMPLES; i++) {
            train_generator(&generator, &discriminator);
        }
        
        if (epoch % 100 == 0) {
            printf("Epoch %d\n", epoch);
        }
    }
    
    printf("Training complete!\n");
    
    return 0;
}