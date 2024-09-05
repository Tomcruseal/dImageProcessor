#include<iostream>
#include<string>
#include<thread>
#include<vector>
#include<fstream>
#include<functional>
#include<cstdlib>

#include<boost/beast/core.hpp>
#include<boost/beast/websocket.hpp>
#include<boost/asio/ip/tcp.hpp>
#include<boost/beast/http.hpp>

using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace http = boost::beast::http;

class ImageProducer{
private:
    net::io_context ioc_;  //I/O context
    tcp::resolver resolver_;  //resolver
    tcp::socket socket_;  //socket

public:
    //constructor to initialize ImageProducer with server address and port, which are to be received from the command line input
    ImageProducer(const std::string& server, const std::string& port) : ioc_(), resolver_(ioc_), socket_(ioc_){
        auto const results = resolver_.resolve(server, port);
        net::connect(socket_, results.begin(), results.end());
    }
    
    //destructor to close the socket
    ~ImageProducer(){
        beast::error_code errorCode;
        socket_.shutdown(tcp::socket::shutdown_both, errorCode);
        if(errorCode && errorCode!=beast::errc::not_connected){
            throw beast::system_error(errorCode);
        }
    }
    
    //send the image to the receiver
    std::string sendImage(const std::string& imagePath){
        //open the image (e.g., jpg) file as a binary
        std::ifstream image(imagePath, std::ios::binary);
        if(!image){
            throw std::runtime_error("Could not open the image file: " + imagePath);
        }
        
        //read the image
        std::string image_data((std::istreambuf_iterator<char>(image)), std::istreambuf_iterator<char>());

        //create and send a http post
        http::request<http::string_body> req{http::verb::post, "/", 11};  //TODO version could also be 12?
        req.set(http::field::host, "localhost");
        req.set(http::field::content_type, "application/octet-stream");
        //move semantic to the body for better performance
        req.body()=std::move(image_data);
        req.prepare_payload();

        http::write(socket_, req);
        
        //buffer for the reponse
        beast::flat_buffer buffer;
        
        //response container
        http::response<http::string_body> res;

        http::read(socket_, buffer, res);
        
        //return the body - it should be entries of pixel colour
        return res.body();

    }


};

int main() {
    std::string input;
    std::string server;
    std::string port;
    std::string imagePath;
    try{
        while(true){
            std::cout<<"Please specify the server address, the port, and the image path. To quit this program, please type 'exit'. \n";
            std::getline(std::cin, input);

            if(input == "exit"){
                std::cout<<"Exiting. "<<std::endl;
                break;
            }

            std::istringstream is(input);
            if(!(is >> server >> port >> imagePath)){
                std::cerr<<"Invalid input. Please check it. "<<std::endl;
                continue;
            }

            ImageProducer imageProducer(server, port);
            std::string result = imageProducer.sendImage(imagePath);

            if(!result.empty()){
                std::cout<<"The calculation result of the most common pixel colour per image row is: \n";
                std::cout<<result<<std::endl;
            }
            else{
                std::cerr<<"Failed to get the result "<<std::endl;
            }

            char choice;
            std::cout<<"Do you want to send another image? (y/n): \n";
            std::cin>>choice;
            std::cin.ignore();

            if(choice !='y' && choice !='Y'){
                std::cout<<"Exiting. "<<std::endl;
                break;
            }
        }

    }catch(const std::exception& e){
        std::cerr<<"Error: "<<e.what()<<std::endl;
    }

    return 0;
}
