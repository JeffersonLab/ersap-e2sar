import org.jlab.epsci.ersap.engine.EngineData;
import org.jlab.epsci.ersap.engine.EngineDataType;
import org.jlab.epsci.ersap.std.services.AbstractEventReaderService;
import org.jlab.epsci.ersap.std.services.EventReaderException;
import org.json.JSONObject;

import java.io.File;  // Import the File class
import java.io.FileNotFoundException;  // Import this class to handle errors
import java.util.Arrays;
import java.io.BufferedReader;
import java.io.FileReader;


import java.io.IOException;
import java.nio.ByteOrder;
import java.nio.file.Path;
import java.nio.file.Paths;
public class FileReaderService extends AbstractEventReaderService<BufferedReader> {

    /**
     * Creates a new image reader service.
     */
    public FileReaderService() {
    }

    @Override
    protected BufferedReader createReader(Path file, JSONObject opts) throws EventReaderException {
        try {
            return new BufferedReader(new FileReader(file.toFile()));
        }
        catch (FileNotFoundException e) {
            throw new EventReaderException("Could not create reader", e);
        }
    }

    @Override
    protected void closeReader() {
        try {
            reader.close();
        } catch (Exception e) {
            // ignore
        }
    }

    @Override
    protected int readEventCount() throws EventReaderException {
        return 0;
    }

    @Override
    protected ByteOrder readByteOrder() throws EventReaderException {
        return ByteOrder.BIG_ENDIAN;
    }

    @Override
    protected Object readEvent(int eventNumber) throws EventReaderException {
        String line;
        try{
            if((line = reader.readLine()) != null){
                return line.getBytes();
            }
        }
        catch (IOException e){
            e.printStackTrace();
        }
        return null;
    }

    @Override
    protected EngineDataType getDataType() {
        return EngineDataType.BYTES;
    }

    public static void main(String[] args) throws Exception {
        Path inputDataSet = Paths.get("/home/ububtu/ersap-e2sar-in.txt");

        FileReaderService reader = new FileReaderService();

        JSONObject config = new JSONObject();
        config.put("action", "open");
        config.put("file", inputDataSet.toString());

        EngineData configData = new EngineData();
        configData.setData(EngineDataType.JSON, config.toString());

        reader.configure(configData);

        byte[] byteArr= (byte[])reader.readEvent(0);
        System.out.println(Arrays.toString(byteArr));

        reader.destroy();
    }
}