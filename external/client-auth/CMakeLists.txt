cmake_minimum_required(VERSION 3.15)

include(FetchContent)
FetchContent_Declare(client-auth
    GIT_REPOSITORY git@github.com:callofduty4x/client-auth.git
    GIT_TAG        39b9637e9391072ce2bd1beded38da80ac52d3ae
)
FetchContent_MakeAvailable(client-auth)
