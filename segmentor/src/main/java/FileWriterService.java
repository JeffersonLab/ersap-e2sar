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
public class FileWriterService extends AbstractEventWriterService<BufferedWriter> {

    public FileWriterService(){

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
            writer.close();
        } catch (Exception e) {
            // ignore
        }
    }

    @Override
    protected void writeEvent(Object event) throws EventWriterException {
        System.out.println("This does nothing");
    }

    @Override
    protected EngineDataType getDataType() {
        return EngineDataType.BYTES;
    }
}