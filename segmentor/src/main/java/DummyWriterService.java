import org.jlab.epsci.ersap.engine.EngineData;
import org.jlab.epsci.ersap.engine.EngineDataType;
import org.jlab.epsci.ersap.std.services.AbstractEventWriterService;
import org.jlab.epsci.ersap.std.services.EventWriterException;
import org.json.JSONObject;

import java.io.File;  // Import the File class
import java.io.FileNotFoundException;  // Import this class to handle errors
import java.io.FileReader;
import java.util.Arrays;

import javax.imageio.ImageWriter;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileWriter;


import java.io.IOException;
import java.nio.ByteOrder;
import java.nio.file.Path;
import java.nio.file.Paths;

/**
 * A service that writes images into a ZIP file.
 */
public class DummyWriterService extends AbstractEventWriterService<BufferedWriter> {
    int count;
    public DummyWriterService(){
        count = 0;
    }
    @Override
    protected BufferedWriter createWriter(Path file, JSONObject opts) throws EventWriterException {
        try {
            return new BufferedWriter(new FileWriter(file.toFile()));
        }
        catch (FileNotFoundException e) {
            throw new EventWriterException("Could not create reader", e);
        }
        catch(IOException e){
            e.printStackTrace();
        }
        return null;
    }

    @Override
    protected void closeWriter() {
        try {
            System.out.println("Number of events sent = " + count);
            writer.close();
        } catch (Exception e) {
            // ignore
        }
    }

    @Override
    protected void writeEvent(Object event) throws EventWriterException {
        count++;
    }

    @Override
    protected EngineDataType getDataType() {
        return EngineDataType.BYTES;
    }
}