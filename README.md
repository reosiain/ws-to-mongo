# ws-to-mongo

Required env variables 
* WS_ADDRESS - address of WesSocket connection
* MONGO_URI - uri to Mongo Instance
* SUBSCRIPTION_PARAMS - path to json containing on-conntect message to ws server
* DB_NAME - name of db containing collection for OHLC data
* OHLC_COLLECTION_NAME - collection name within db for OHLC data
* (optional) VERBOSE_MSG - prints incoming messages if declared