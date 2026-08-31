# Web server
Deploy a website using this **web server** made in C++ from scratch! <br/>
A **socket server** to take incoming requests is also embedded directly in the project. <br/>
To easily handle back-end with this web server, you have a new custom tag at your disposal for your HTML pages. <br/>
**DO NOT RUN THIS PROJECT WITH ADMINISTRATOR PRIVILILEGES.** If you have binding issues with the configured port, simply allow the web_server(.exe) binary to use this specific port.

# 🕸️ Add new routes
To add new routes to the project, you just have to register them in the `register_routes` function the `routes/routes.cpp` script. <br/>
For example, to create the page `/demo` that runs the function `demo_page` when it's requested, we add it to the function:
```c++
void Routes::register_routes()
{
    routes["/"] = []() -> std::string
    {
        return Pages::page_home();
    };

    routes["/home"] = []() -> std::string
    {
        return Pages::page_home();
    };
    
    routes["/demo"] = []() -> std::string
    {
        return Pages::page_demo();
    };
}
```
Do not forget to declare the function and add it to the `Pages` namespace in the `routes/pages` (*if you wish to follow the infrastructure as it*). <br/>
And also keep in mind that your page **MUST** return a string. This string will be used as the **page to display** back to the client. <br/>
This is why the default pages `/` and `/home` return HTML code.

# ⚒️ Custom tag
To build the back-end of your website in C++, this project offers you a new tag to directly use in your HTML pages: **`<++  ++>`**. <br/>
To use it, you put the tag in your HTML page, and give it a name. <br/>
For instance, if we want to create a tag named "**test**", we write this directly into our HTML page: **`<++ test ++>`**. <br/>
Once it's done, in the function that you associated to your specific route, to replace the tag with actual data, you define a vector list of `BackendData` objects. <br/>
Afterwards, you simply have to replace the tags with some data using the helper function `replace_custom_tags`. <br/>
Let's imagine I want to replace my test tag with the message "**Hello World!**":
``` c++
// Format: backend_data["tag_name"] = "data".
const std::vector<BackendData> backend_data
{
    { "test", "Hello World!" }
};

std::vector<std::string> html_page = Files::read_file("./website/page.html");       // Read your HTML file content.
const std::string rendering = Routes::replace_custom_tags(backend_data, html_page); // Replace all tags with the data defined in the vector list above.
```
Real case example: https://github.com/Arrrkorrr/web-server/blob/master/routes/pages/home.cpp.

# ⚙️ Configuration
You can configure the web server using the `./config/server.config` file. <br/>
The default configuration is available at the bottom of this section. <br/>
Here are the different options you have: <br/>
- `"ADDRESS"` -> Address to deploy the server to.
- `"PORT"` -> Port to listen on the specified address.
- `"MAX_RETRIES"` -> Amount of time the socket server tries to listen the port before aborting.
- `"MAX_REQUEST_LENGTH"` -> Maximum length of a request. Keep in mind that your operating system might still set a hard limit for file names.
``` config
ADDRESS=127.0.0.1
PORT=8080
MAX_RETRIES=5
MAX_REQUEST_LENGTH=100
```


# 📦 Requirements
- CMake compiler.
- GCC/G++.

# 📥 Installation
1) Download the project. <br/>
2) Build your website with it. <br/>
3) Compile it using `build.sh` or `build.bat`. <br/>
4) Enjoy!
> To compile for Windows systems using `build.bat`, verify the cmake/g++ compiler paths.

# 🤝 User Agreement
By downloading and/or using this program, you confirm that you are solely responsible for how you use this software and you accept the MIT License, available in the `LICENSE.md` file. You agree as well that this agreement extends to any prior version of the program, and any new version of the user agreement in any future update, overwrites this one.
