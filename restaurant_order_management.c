#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ITEMS 100
#define MAX_NAME_LENGTH 50
#define MAX_ORDER_ITEMS 10

struct MenuItem {
    char name[MAX_NAME_LENGTH];
    float price;
};

struct OrderItem {
    char itemName[MAX_NAME_LENGTH];
    int quantity;
};

struct Order {
    int orderNumber;
    struct tm orderTime;
    struct OrderItem items[MAX_ORDER_ITEMS];
    int numItems;
};

struct Node {
    struct Order data;
    struct Node* next;
};

struct Queue {
    struct Node *front, *rear;
};

int orderCounter = 1;

// Node creation
struct Node* createNode(struct Order item) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = item;
    newNode->next = NULL;
    return newNode;
}

// Create queue
struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

// Check if queue is empty
int isEmpty(struct Queue* queue) {
    return queue->front == NULL;
}

// Enqueue
void enqueue(struct Queue* queue, struct Order item) {
    struct Node* newNode = createNode(item);
    if (isEmpty(queue)) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Dequeue
struct Order dequeue(struct Queue* queue) {
    struct Order emptyOrder;
    emptyOrder.orderNumber = -1;

    if (isEmpty(queue)) {
        return emptyOrder;
    }

    struct Node* temp = queue->front;
    struct Order item = temp->data;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    return item;
}

// Display menu
void displayMenu(struct MenuItem menu[], int n) {
    printf("\nMenu:\n");
    for (int i = 0; i < n; i++) {
        printf("%d. %s - Rs%.2f\n", i + 1, menu[i].name, menu[i].price);
    }
}

// Input menu items
void inputMenu(struct MenuItem menu[], int* numItems) {
    printf("Enter the number of menu items: ");
    scanf("%d", numItems);
    for (int i = 0; i < *numItems; i++) {
        printf("Item %d Name: ", i + 1);
        scanf(" %[^\n]s", menu[i].name);
        printf("Item %d Price: Rs", i + 1);
        scanf("%f", &menu[i].price);
    }
}

// Place order
void placeOrder(struct MenuItem menu[], int n, struct Queue* orderQueue) {
    displayMenu(menu, n);
    struct Order newOrder;
    newOrder.orderNumber = orderCounter++;
    newOrder.numItems = 0;

    printf("Enter number of items in order: ");
    int numItemsInOrder;
    scanf("%d", &numItemsInOrder);
    if (numItemsInOrder < 1 || numItemsInOrder > MAX_ORDER_ITEMS) {
        printf("Invalid number of items!\n");
        return;
    }

    time_t currentTime;
    time(&currentTime);
    newOrder.orderTime = *localtime(&currentTime);

    for (int i = 0; i < numItemsInOrder; i++) {
        printf("Enter item number and quantity for item %d: ", i + 1);
        int itemNum, quantity;
        scanf("%d %d", &itemNum, &quantity);
        if (itemNum < 1 || itemNum > n) {
            printf("Invalid item number!\n");
            return;
        }
        strcpy(newOrder.items[i].itemName, menu[itemNum - 1].name);
        newOrder.items[i].quantity = quantity;
        newOrder.numItems++;
    }

    enqueue(orderQueue, newOrder);
    printf("Order placed successfully! Order Number: %d\n", newOrder.orderNumber);
}

// Prepare order
void prepareOrder(struct Queue* orderQueue, struct Queue* kitchenQueue) {
    struct Order preparedOrder = dequeue(orderQueue);
    if (preparedOrder.orderNumber == -1) {
        printf("No orders to prepare!\n");
        return;
    }
    enqueue(kitchenQueue, preparedOrder);
    printf("Order %d prepared and sent to kitchen.\n", preparedOrder.orderNumber);
}

// Serve order
void serveOrder(struct Queue* kitchenQueue) {
    struct Order servedOrder = dequeue(kitchenQueue);
    if (servedOrder.orderNumber == -1) {
        printf("No orders to serve!\n");
        return;
    }
    printf("Serving Order #%d:\n", servedOrder.orderNumber);
    for (int i = 0; i < servedOrder.numItems; i++) {
        printf("Item: %s, Quantity: %d\n", servedOrder.items[i].itemName, servedOrder.items[i].quantity);
    }
    printf("Order Time: %d-%02d-%02d %02d:%02d:%02d\n",
           servedOrder.orderTime.tm_year + 1900, servedOrder.orderTime.tm_mon + 1, servedOrder.orderTime.tm_mday,
           servedOrder.orderTime.tm_hour, servedOrder.orderTime.tm_min, servedOrder.orderTime.tm_sec);
}

// Calculate total
float calculateOrderTotal(struct Order order, struct MenuItem menu[], int menuSize) {
    float total = 0.0;
    for (int i = 0; i < order.numItems; i++) {
        for (int j = 0; j < menuSize; j++) {
            if (strcmp(order.items[i].itemName, menu[j].name) == 0) {
                total += menu[j].price * order.items[i].quantity;
                break;
            }
        }
    }
    return total;
}

// Print receipt
void printReceipt(struct Order order, struct MenuItem menu[], int menuSize) {
    printf("\n--- Receipt for Order #%d ---\n", order.orderNumber);
    for (int i = 0; i < order.numItems; i++) {
        printf("Item: %s - Quantity: %d\n", order.items[i].itemName, order.items[i].quantity);
    }
    printf("Order Time: %d-%02d-%02d %02d:%02d:%02d\n",
           order.orderTime.tm_year + 1900, order.orderTime.tm_mon + 1, order.orderTime.tm_mday,
           order.orderTime.tm_hour, order.orderTime.tm_min, order.orderTime.tm_sec);
    printf("Total Amount: Rs%.2f\n", calculateOrderTotal(order, menu, menuSize));
}

// Search order in queue
struct Node* searchOrder(struct Queue* queue, int orderNumber) {
    struct Node* temp = queue->front;
    while (temp != NULL) {
        if (temp->data.orderNumber == orderNumber)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

// Free queue memory
void freeQueue(struct Queue* queue) {
    while (!isEmpty(queue)) {
        dequeue(queue);
    }
    free(queue);
}

// Main
int main() {
    struct MenuItem menu[MAX_ITEMS];
    int numItems = 0;

    struct Queue* orderQueue = createQueue();
    struct Queue* kitchenQueue = createQueue();

    int choice;
    do {
        printf("\n--- Restaurant Order Management ---\n");
        printf("1. Display Menu\n");
        printf("2. Input Menu\n");
        printf("3. Place Order\n");
        printf("4. Prepare Order\n");
        printf("5. Serve Order\n");
        printf("6. Print Receipt\n");
        printf("7. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayMenu(menu, numItems);
                break;
            case 2:
                inputMenu(menu, &numItems);
                break;
            case 3:
                placeOrder(menu, numItems, orderQueue);
                break;
            case 4:
                prepareOrder(orderQueue, kitchenQueue);
                break;
            case 5:
                serveOrder(kitchenQueue);
                break;
            case 6: {
                int orderNumber;
                printf("Enter Order Number: ");
                scanf("%d", &orderNumber);
                struct Node* result = searchOrder(orderQueue, orderNumber);
                if (result == NULL)
                    result = searchOrder(kitchenQueue, orderNumber);
                if (result != NULL)
                    printReceipt(result->data, menu, numItems);
                else
                    printf("Order not found!\n");
                break;
            }
            case 7:
                printf("Thank you! Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 7);

    freeQueue(orderQueue);
    freeQueue(kitchenQueue);
    return 0;
}
