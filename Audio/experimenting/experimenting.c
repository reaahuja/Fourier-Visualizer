#include <pthread.h>
#include <stdio.h>
#include <unistd.h> // For sleep()

// Function to be executed by the first thread
void* function1(void* arg) {
    for (int i = 0; i < 5; i++) {
        printf("Function 1 is running\n");
        sleep(1); // Sleep for 1 second
    }
    return NULL;
}

// Function to be executed by the second thread
void* function2(void* arg) {
    for (int i = 0; i < 5; i++) {
        printf("Function 2 is running\n");
        sleep(1); // Sleep for 1 second
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // Create the first thread
    if (pthread_create(&thread1, NULL, function1, NULL) != 0) {
        printf("Failed to create thread 1\n");
        return 1;
    }

    // Create the second thread
    if (pthread_create(&thread2, NULL, function2, NULL) != 0) {
        printf("Failed to create thread 2\n");
        return 1;
    }

    // Wait for both threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Both functions have finished executing\n");

    return 0;
}
