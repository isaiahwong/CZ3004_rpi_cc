class Movement {
   private:
    inline static const std::string RIGHT_90 = "57";
    inline static const std::string RIGHT_180 = "114";
    inline static const std::string LEFT_90 = "28";
    inline static const std::string LEFT_180 = "59";

   public:
    typedef void (*WriteCallback)(void* cereal, std::string);

    void forward() {}

    void back() {}

    void left() {}

    void right() {}
};