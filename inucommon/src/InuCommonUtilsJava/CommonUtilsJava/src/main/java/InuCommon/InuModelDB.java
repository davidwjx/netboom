package InuCommon;

import java.util.HashMap;

/**
 * Java wrapper to Sensor models
 * 
 * Role: Stores pre-defined parameters that are loaded from internal config file for all Sensor models  
 * Responsibilities:
 * 1. Knows all Sensor models that are available
 * 2. Knows how to load these models from internal config file
 * 
 * @author Olga
 * @since 2.09.0004
 */

public class InuModelDB {
	
	public InuModelDB()
	{
		mInuModels = new HashMap<String, InuModel>();
	}
    /** 
     * sensor models DB
     */
    public HashMap<String, InuModel> mInuModels;

    /**
     * Initialize the sensor models DB object.
     * InuModel attributes are read from internal configuration file
     * 
     * @return false if the object could not be loaded
     */
    public native boolean init();
}   
