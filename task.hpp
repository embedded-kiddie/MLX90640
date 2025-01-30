/*================================================================================
 * Multitasking helper functions
 *================================================================================*/
#define TASK1_CORE      1
#define TASK2_CORE      0

#define TASK1_STACK     8192
#define TASK2_STACK     8192

#define TASK1_PRIORITY  2
#define TASK2_PRIORITY  1

// Message queue sent from task 1 to task 2
typedef struct {
  uint8_t   bank;   // Exclusive bank numbers for Task 1 and Task 2
  uint32_t  start;  // Task 1 start time
  uint32_t  finish; // Task 1 Finish Time
} MessageQueue_t;

// Define two tasks on the core
void Task1(void *pvParameters);
void Task2(void *pvParameters);

// Define pointers to the tasks
static void (*Process1)(uint8_t bank);
static void (*Process2)(uint8_t bank, uint32_t start, uint32_t finish);

// Message queues and semaphores for handshaking
static TaskHandle_t taskHandle[2];
static QueueHandle_t queHandle;
static SemaphoreHandle_t semHandle;

#define HALT()      { for(;;) delay(1000); }
#define WAIT_QUEUE  portTICK_PERIOD_MS  // portMAX_DELAY
#define NOT_UPDATED 0xFF

// The setup function runs once when press reset or power on the board
void task_setup(void (*task1)(uint8_t), void (*task2)(uint8_t, uint32_t, uint32_t)) {
  // Pointers to the tasks to be executed.
  Process1 = task1;
  Process2 = task2;

  // Task1 start immediately --> Process1 --> Task1 send queue --> Task1 wait for semaphre
  // --> Task2 receive queue --> Process2 --> Task2 give semaphre --> Task2 wait queue
  // --> Task1 take sepahore --> Process1 --> ...
  semHandle = xSemaphoreCreateCounting(1, TASK1_CORE != TASK2_CORE ? 0 : 0);
  queHandle = xQueueCreate(1, sizeof(MessageQueue_t));

  // Check if the queue or the semaphore was successfully created
  if (queHandle == NULL || semHandle == NULL) {
    DBG_EXEC(printf("Can't create queue or semaphore.\n"));
    HALT();
  }

  // Set up sender task in core 1 and start immediately
  xTaskCreatePinnedToCore(
    Task1, "Task1",
    TASK1_STACK,    // The stack size
    NULL,           // Pass reference to a variable describing the task number
    TASK1_PRIORITY, // priority
    &taskHandle[0], // Pass reference to task handle
    TASK1_CORE
  );

  // Set up receiver task on core 0 and start immediately
  xTaskCreatePinnedToCore(
    Task2, "Task2",
    TASK2_STACK,    // The stack size
    NULL,           // Pass reference to a variable describing the task number
    TASK2_PRIORITY, // priority
    &taskHandle[1], // Pass reference to task handle
    TASK2_CORE
  );
}

/*--------------------------------------------------*/
/*------------------- Handshake --------------------*/
/*--------------------------------------------------*/
uint8_t SendQueue(uint8_t bank, uint32_t start, uint32_t finish) {
  MessageQueue_t queue = {
    bank, start, finish
  };

  if (xQueueSend(queHandle, &queue, portMAX_DELAY) == pdTRUE) {
//  DBG_EXEC(printf("Give queue: %d\n", queue.bank));
  } else {
    DBG_EXEC(printf("unable to send queue\n"));
  }

  return !bank;
}

MessageQueue_t ReceiveQueue() {
  MessageQueue_t queue;

  if (xQueueReceive(queHandle, &queue, portMAX_DELAY) == pdTRUE) {
//  DBG_EXEC(printf("Take queue: %d\n", queue.bank));
  } else {
    DBG_EXEC(printf("Unable to receive queue.\n"));
  }

  return queue;
}

bool ScanQueue(MessageQueue_t &queue) {
  if (xQueueReceive(queHandle, &queue, WAIT_QUEUE) == pdTRUE) {
    return true;
  } else {
    return false;
  }
}

void TakeSemaphore(void) {
  if (xSemaphoreTake(semHandle, portMAX_DELAY) == pdTRUE) {
//  DBG_EXEC(printf("Take semaphore.\n"));
  } else {
    DBG_EXEC(printf("Unable to take semaphore.\n"));
  }
}

void GiveSemaphore(void) {
  if (xSemaphoreGive(semHandle) == pdTRUE) {
//  DBG_EXEC(printf("Give semaphore.\n"));
  } else {
    DBG_EXEC(printf("Unable to give semaphore.\n"));
  }
}

/*--------------------------------------------------*/
/*--------------------- Tasks ----------------------*/
/*--------------------------------------------------*/
void Task1(void *pvParameters) {
  uint8_t bank = 0;

  while (true) {
    uint32_t start = millis();

    Process1(bank);

    bank = SendQueue(bank, start, millis());

    TakeSemaphore();
  }
}

void Task2(void *pvParameters) {
  MessageQueue_t queue;

  while (true) {
    if (ScanQueue(queue)) {
      GiveSemaphore();
    } else {
      queue.bank = NOT_UPDATED;
    }

    Process2(queue.bank, queue.start, queue.finish);

    yield(); // Prevent the watchdog from firing
  }
}
