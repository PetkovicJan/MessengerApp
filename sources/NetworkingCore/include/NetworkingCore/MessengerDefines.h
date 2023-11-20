#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class AppMessageType { UserCreated, UserLoggedIn, UserLoggedOut, UserSentMessage, UserCreatedStatus, UserLoginStatus };

enum class CreateStatus { Success, Failure };

enum class LoginStatus { Success, InvalidUsername, InvalidPassword };