import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpServer;
import com.sun.net.httpserver.HttpsExchange;
import com.sun.net.httpserver.HttpHandler;

import javax.imageio.ImageIO;
import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;

import java.util.HashMap;
import java.util.Map;
import java.util.List;
import java.util.Vector;

import org.json.JSONArray;
import org.json.JSONObject;

public class ImageConsumer {

    public static void main(String[] args) throws Exception {
        HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);  //create a socket addr with port 8080 which correponds to the port specified by the ImageProducer
        server.createContext("/", new ImageProcessor());
        server.setExecutor(null);
        server.start();
        System.out.println("ImageConsumer is listening on port 8080");
    }

    static class ImageProcessor implements HttpHandler {
        @Override
        public void handle(HttpExchange t) throws IOException {
            if("POST".equals(t.getRequestMethod())) {
                InputStream is = t.getRequestBody();
                byte[] buffer = is.readAllBytes();
                BufferedImage image = ImageIO.read(new ByteArrayInputStream(buffer));  //get the image from request
                JSONArray result = imageCalculate(image);  //calculate the most common pixel colour per image row
                String jsonRes = result.toString();  //convert result to String
                t.getResponseHeaders().add("Content-Type", "application/json");  //send back the result to the ImageProducer
                t.sendResponseHeaders(200, jsonRes.getBytes().length);  //status code 200, all good  
                OutputStream os = t.getResponseBody();
                os.write(jsonRes.getBytes());
                os.close();
            }
            else{
                t.sendResponseHeaders(405, -1);  //status code 405 not good
            }
        }

        private JSONArray imageCalculate(BufferedImage image) {
            int width = image.getWidth();
            int height = image.getHeight();
            JSONArray result = new JSONArray();

            for(int i = 0; i < height; i++) {
                Map<Integer, Integer> colorCount = new HashMap<>();
                for(int j = 0; j < width; j++) {
                    int rgb = image.getRGB(j, i);
                    colorCount.put(rgb, colorCount.getOrDefault(rgb, 0) + 1);
                }

                //int mostCommonColor = colorCount.entrySet().stream().max(Map.Entry.comparingByValue()).get().getKey();
                int mostCommonColor = 0;
                //a special case when every pixel colour is unique, then we choose to return the first pixel's colour as the most common one
                if(colorCount.size()==width){
                    mostCommonColor = image.getRGB(0, i);
                }
                else{
                    mostCommonColor = colorCount.entrySet().stream().max(Map.Entry.comparingByValue()).get().getKey();  //get the 'most common' one
                }

                Color color = new Color(mostCommonColor);
                JSONObject colorJson = new JSONObject();
                colorJson.put("r", color.getRed());
                colorJson.put("g", color.getGreen());
                colorJson.put("b", color.getBlue());
                result.put(colorJson);
            }
            return result;
        }
    }
}
