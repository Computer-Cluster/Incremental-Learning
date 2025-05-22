#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Definición de parámetros
#define IMAGE_SIZE 8       // 8x8 imágenes en el dataset óptico
#define NUM_CLASSES 10     // Dígitos del 0 al 9
#define NUM_TRAIN 1000     // Número de ejemplos de entrenamiento
#define NUM_TEST 200       // Número de ejemplos de prueba
#define LEARNING_RATE 0.01
#define EPOCHS 50
#define BATCH_SIZE 32

// Estructura para la CNN
typedef struct {
    // Capa convolucional
    float conv_weights[3][3][1][4]; // [filter_h][filter_w][in_channels][out_channels]
    float conv_biases[4];
    float conv_output[6][6][4];     // (8-3+1)x(8-3+1)x4
    
    // Capa ReLU
    float relu_output[6][6][4];
    
    // Capa Max Pooling
    float pool_output[3][3][4];     // (6/2)x(6/2)x4
    
    // Capa totalmente conectada
    float fc_weights[3*3*4][NUM_CLASSES];
    float fc_biases[NUM_CLASSES];
    
    // Salida final
    float output[NUM_CLASSES];
} CNN;

// Función de inicialización
void initialize_cnn(CNN *cnn) {
    srand(time(0));
    
    // Inicializar pesos convolucionales
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 1; k++) {
                for (int l = 0; l < 4; l++) {
                    cnn->conv_weights[i][j][k][l] = ((float)rand() / RAND_MAX) * 0.1;
                }
            }
        }
    }
    
    // Inicializar biases convolucionales
    for (int i = 0; i < 4; i++) {
        cnn->conv_biases[i] = 0.0;
    }
    
    // Inicializar pesos FC
    for (int i = 0; i < 3*3*4; i++) {
        for (int j = 0; j < NUM_CLASSES; j++) {
            cnn->fc_weights[i][j] = ((float)rand() / RAND_MAX) * 0.1;
        }
    }
    
    // Inicializar biases FC
    for (int i = 0; i < NUM_CLASSES; i++) {
        cnn->fc_biases[i] = 0.0;
    }
}

// Función de convolución
void convolution(float input[IMAGE_SIZE][IMAGE_SIZE], CNN *cnn) {
    int output_size = IMAGE_SIZE - 3 + 1; // 6x6
    
    for (int filter = 0; filter < 4; filter++) {
        for (int i = 0; i < output_size; i++) {
            for (int j = 0; j < output_size; j++) {
                float sum = 0.0;
                
                for (int ki = 0; ki < 3; ki++) {
                    for (int kj = 0; kj < 3; kj++) {
                        sum += input[i + ki][j + kj] * cnn->conv_weights[ki][kj][0][filter];
                    }
                }
                
                cnn->conv_output[i][j][filter] = sum + cnn->conv_biases[filter];
            }
        }
    }
}

// Función ReLU
void relu(CNN *cnn) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 4; k++) {
                cnn->relu_output[i][j][k] = fmaxf(0.0, cnn->conv_output[i][j][k]);
            }
        }
    }
}

// Función Max Pooling
void max_pooling(CNN *cnn) {
    for (int filter = 0; filter < 4; filter++) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                float max_val = -INFINITY;
                
                for (int ki = 0; ki < 2; ki++) {
                    for (int kj = 0; kj < 2; kj++) {
                        max_val = fmaxf(max_val, cnn->relu_output[i*2 + ki][j*2 + kj][filter]);
                    }
                }
                
                cnn->pool_output[i][j][filter] = max_val;
            }
        }
    }
}

// Función Softmax
void softmax(CNN *cnn) {
    float max_val = -INFINITY;
    float sum = 0.0;
    
    // Encontrar el valor máximo para estabilidad numérica
    for (int i = 0; i < NUM_CLASSES; i++) {
        max_val = fmaxf(max_val, cnn->output[i]);
    }
    
    // Calcular exponenciales y suma
    for (int i = 0; i < NUM_CLASSES; i++) {
        cnn->output[i] = exp(cnn->output[i] - max_val);
        sum += cnn->output[i];
    }
    
    // Normalizar
    for (int i = 0; i < NUM_CLASSES; i++) {
        cnn->output[i] /= sum;
    }
}

// Función de propagación hacia adelante
void forward_pass(float input[IMAGE_SIZE][IMAGE_SIZE], CNN *cnn) {
    // Capa convolucional
    convolution(input, cnn);
    
    // ReLU
    relu(cnn);
    
    // Max Pooling
    max_pooling(cnn);
    
    // Aplanar para la capa FC
    float flattened[3*3*4];
    int idx = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 4; k++) {
                flattened[idx++] = cnn->pool_output[i][j][k];
            }
        }
    }
    
    // Capa totalmente conectada
    for (int i = 0; i < NUM_CLASSES; i++) {
        cnn->output[i] = cnn->fc_biases[i];
        for (int j = 0; j < 3*3*4; j++) {
            cnn->output[i] += flattened[j] * cnn->fc_weights[j][i];
        }
    }
    
    // Softmax
    softmax(cnn);
}

// Función de pérdida (entropía cruzada)
float cross_entropy_loss(CNN *cnn, int label) {
    return -log(cnn->output[label] + 1e-10); // Pequeño valor para evitar log(0)
}

// Función para cargar el dataset (simplificado)
void load_dataset(float train_data[NUM_TRAIN][IMAGE_SIZE][IMAGE_SIZE], int train_labels[NUM_TRAIN],
                  float test_data[NUM_TEST][IMAGE_SIZE][IMAGE_SIZE], int test_labels[NUM_TEST]) {
    // En una implementación real, aquí cargarías los datos de archivos
    // Esto es solo un ejemplo simplificado
    for (int i = 0; i < NUM_TRAIN; i++) {
        for (int j = 0; j < IMAGE_SIZE; j++) {
            for (int k = 0; k < IMAGE_SIZE; k++) {
                train_data[i][j][k] = ((float)rand() / RAND_MAX); // Valores entre 0 y 1
            }
        }
        train_labels[i] = rand() % NUM_CLASSES;
    }
    
    for (int i = 0; i < NUM_TEST; i++) {
        for (int j = 0; j < IMAGE_SIZE; j++) {
            for (int k = 0; k < IMAGE_SIZE; k++) {
                test_data[i][j][k] = ((float)rand() / RAND_MAX); // Valores entre 0 y 1
            }
        }
        test_labels[i] = rand() % NUM_CLASSES;
    }
}

// Función principal
int main() {
    // Inicializar CNN
    CNN cnn;
    initialize_cnn(&cnn);
    
    // Cargar dataset
    float train_data[NUM_TRAIN][IMAGE_SIZE][IMAGE_SIZE];
    int train_labels[NUM_TRAIN];
    float test_data[NUM_TEST][IMAGE_SIZE][IMAGE_SIZE];
    int test_labels[NUM_TEST];
    
    load_dataset(train_data, train_labels, test_data, test_labels);
    
    // Entrenamiento
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        float total_loss = 0.0;
        int correct = 0;
        
        for (int i = 0; i < NUM_TRAIN; i++) {
            // Propagación hacia adelante
            forward_pass(train_data[i], &cnn);
            
            // Calcular pérdida
            total_loss += cross_entropy_loss(&cnn, train_labels[i]);
            
            // Calcular precisión
            int predicted = 0;
            float max_prob = 0.0;
            for (int j = 0; j < NUM_CLASSES; j++) {
                if (cnn.output[j] > max_prob) {
                    max_prob = cnn.output[j];
                    predicted = j;
                }
            }
            
            if (predicted == train_labels[i]) {
                correct++;
            }
            
            // Aquí iría la retropropagación (backpropagation) en una implementación completa
            // Se omite por simplicidad en este ejemplo
        }
        
        printf("Epoch %d, Loss: %.4f, Accuracy: %.2f%%\n", 
               epoch + 1, total_loss / NUM_TRAIN, (float)correct / NUM_TRAIN * 100);
    }
    
    // Evaluación en el conjunto de prueba
    int test_correct = 0;
    for (int i = 0; i < NUM_TEST; i++) {
        forward_pass(test_data[i], &cnn);
        
        int predicted = 0;
        float max_prob = 0.0;
        for (int j = 0; j < NUM_CLASSES; j++) {
            if (cnn.output[j] > max_prob) {
                max_prob = cnn.output[j];
                predicted = j;
            }
        }
        
        if (predicted == test_labels[i]) {
            test_correct++;
        }
    }
    
    printf("\nTest Accuracy: %.2f%%\n", (float)test_correct / NUM_TEST * 100);
    
    return 0;
}