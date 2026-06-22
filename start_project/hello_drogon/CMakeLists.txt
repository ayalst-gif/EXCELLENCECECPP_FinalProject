cmake_minimum_required(VERSION 3.5)
project(hello_drogon CXX)

# הגדרת הסטנדרט הגלובלי
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# הגדרת ה-executable עם קבצי המקור המרכזיים
add_executable(${PROJECT_NAME} 
    main.cc 
    controllers/UsersController.cc 
    services/UserService.cc 
    Repository/DB_Repository.cc 
    Repository/UserRepository.cc
)

# מכריח את ה-Target להשתמש ב-C++17 ומעלה
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

find_package(Drogon CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)

# סריקת התיקיות השונות לקבצי מקור
aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

# *** התיקון הקריטי חלק 1: שמירת רשימת קבצי ה-Views המיוצרים לתוך משתנה בשם VIEW_SRC ***
drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views ${CMAKE_CURRENT_BINARY_DIR} VIEW_SRC)

# הגדרת תיקיות ה-Include
target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/models
                                   ${CMAKE_CURRENT_SOURCE_DIR}/services
                                   ${CMAKE_CURRENT_SOURCE_DIR}/Repository
                                   ${CMAKE_CURRENT_SOURCE_DIR}/controllers
                                   ${CMAKE_CURRENT_SOURCE_DIR}/database
                                   ${CMAKE_CURRENT_BINARY_DIR} # חובה כדי שהשרת ימצא את ה-Headers של ה-Views
)

# *** התיקון הקריטי חלק 2: הוספת ה-VIEW_SRC לתוך הריצה כדי שיקומפלו אל תוך ה-Binary ***
target_sources(${PROJECT_NAME}
               PRIVATE
               ${CTL_SRC}
               ${FILTER_SRC}
               ${PLUGIN_SRC}
               ${MODEL_SRC}
               ${VIEW_SRC} # הוספת ה-Views המקומפלים לכאן!
)

add_subdirectory(test)
