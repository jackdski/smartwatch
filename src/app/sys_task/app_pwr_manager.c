//
// app_pwr_manager.c
//

typedef enum
{
    APP_PWR_MANAGER_OFF,
    APP_PWR_MANAGER_LOW_POWER,
    APP_PWR_MANAGER_ACTIVE,
    APP_PWR_MANAGER_CHARGING,
} app_pwr_manager_state_E;

typedef struct
{
    bool displayOn;
    bool touchActive;
    bool buttonPressed;
    bool charging;
    app_pwr_manager_state_E state;
} app_pwr_manager_data_t;


app_pwr_manager_data_t data;

void app_pwr_manager_updateInputs(void)
{

}

void app_pwr_manager_run(void)
{
    app_pwr_manager_updateInputs();

}

bool app_pwr_manager_shouldSleep(void)
{
    return (data.state == APP_PWR_MANAGER_OFF);
}