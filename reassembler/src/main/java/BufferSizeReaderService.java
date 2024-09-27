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
import java.nio.charset.Charset;
import java.nio.ByteOrder;
import java.nio.ByteBuffer;
import java.nio.file.Path;
import java.nio.file.Paths;
public class BufferSizeReaderService extends AbstractEventReaderService<BufferedReader> {

    int count;
    /**
     * Creates a new image reader service.
     */
    public BufferSizeReaderService() {
        count = 0;
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
        return 1;//Only one event started this should be modified
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
                return Integer.parseInt(line);//First line of input file is the number of lines. So for this example the bufferSize will be that
            }
        }
        catch (IOException e){
            e.printStackTrace();
        }
        return null;
    }

    @Override
    protected EngineDataType getDataType() {
        return EngineDataType.SINT32;
    }
}