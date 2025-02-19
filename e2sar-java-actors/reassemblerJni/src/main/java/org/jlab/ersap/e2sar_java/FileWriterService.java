package org.jlab.ersap.e2sar_java;

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
import java.io.FileOutputStream;


import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.ByteOrder;
import java.nio.ByteBuffer;
import java.nio.file.Path;
import java.nio.file.Paths;

/**
 * A service that writes images into a ZIP file.
 */
public class FileWriterService extends AbstractEventWriterService<FileOutputStream> {
    int count;
    public FileWriterService(){
        count = 0;
    }
    @Override
    protected FileOutputStream createWriter(Path file, JSONObject opts) throws EventWriterException {
        try {
            return new FileOutputStream(file.toFile());
        }
        catch (FileNotFoundException e) {
            throw new EventWriterException("Could not create reader", e);
        }
    }

    @Override
    protected void closeWriter() {
        try {
            writer.close();
        } catch (Exception e) {
            // ignore
        }
        System.out.println("Total number of events: " + count);
    }

    @Override
    protected void writeEvent(Object event) throws EventWriterException {
        count++;
        ByteBuffer buffer = (ByteBuffer) event;
        while(buffer.hasRemaining()){
            try{
                writer.write(buffer.get());
            }
            catch(IOException e){
                e.printStackTrace();
            }
        }
        try{
            writer.write("\n".getBytes());
        }
        catch(IOException e){
            e.printStackTrace();
        }
    }

    @Override
    protected EngineDataType getDataType() {
        return EngineDataType.BYTES;
    }
}