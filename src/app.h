struct App {

    // Public fields

    Point window;

    Point mouse;
    Point scroll;
    bool mouse_left_down;
    bool mouse_right_down;
    bool mouse_middle_down;
    bool mouse_left_pressed;
    bool mouse_right_pressed;
    bool mouse_middle_pressed;

    bool should_quit;


    // Public methods

    bool init(const char *title, int window_width, int window_height);
    void quit();

    void update();

    void clear();
    void present();
}
