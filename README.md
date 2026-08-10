# Web server
Deploy a website using this **socket** and **web server** made in C++ from scratch! <br/>
To easily handle back-end with this web server, you have a new custom tag at your disposal for your HTML pages. <br/>
**DO NOT RUN THIS WEB SERVER AS ADMINISTRATOR OR USING THE SUDO COMMAND.** If you do have binding issues with the port, simply allow the web_server(.exe) binary to listen on the port.

# ⚒️ Custom tag
To build the back-end of your website in C++, this project offers you a new tag to directly use in your HTML pages: **`<++  ++>`**. To use it, you put the tag in your page, and give it a name. For instance, let's say I want to name this tag "*test*", I write this into the HTML page: **`<++ test ++>`**. Once it's done, in the function that you associated to a specific route, to replace this tag with actual data, you define a vector list to add `BackendData` objects to it, and use the helper functions to do the modifications for you. Let's imagine I want to replace my test tag with the message "*Hello World!*":
``` c++
const std::vector<BackendData> backend_data
{
    { "test", "Hello World!" },
};

std::vector<std::string> html_page = Utils::Files::read_file("./website/page.html");
const std::string rendering = Routes::replace_custom_tags(backend_data, html_page);
```
Real case example: https://github.com/ArchorByte/web-server/blob/master/src/routes/pages/home.cpp.

# ⚙️ Configuration
You can configure the web server using the `./config/socket.config` file. <br/>
The default configuration is available at the bottom of this section. <br/>
Here are the different options you have: <br/>
- `"ADDRESS"` -> Address to deploy the server to.
- `"PORT"` -> Port to listen on the specified address.
- `"MAX_RETRIES"` -> Amount of time the socket server tries to listen the port before aborting.
``` config
ADDRESS=127.0.0.1
PORT=8080
MAX_RETRIES=5
```


# 📦 Requirements
- CMake compiler.
- GCC/G++ (**C++ 17**).

# 📥 Installation
1) Download the project. <br/>
2) Build your website with it. <br/>
3) Compile it using `build.sh` or `build.bat` for simplicity. <br/>
4) Enjoy!
> To compile for Windows systems using `build.bat`, verify your cmake/g++ compiler paths.

# 🤝 User Agreement
By downloading and/or using this program, you confirm that you are solely responsible for how you use this software and you accept the MIT License, available in the `LICENSE.md` file. You agree as well that this agreement extends to any prior version of the program, and any new version of the user agreement in any future update, overwrites this one.
