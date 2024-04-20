import {MongoClient} from "mongodb"
export default class Database{
    connection: any;
    db: any;
    collection: any;

    constructor(){
        this.connection = null;
        this.db = null;
        this.collection = null;
    }

    /**
     * Initialize connection to a collection in a database
     * @param url - the url to the database server 
     * @param dbName - name of the database
     * @param collection - name of the collection
     */
    async connect(url, dbName, collection){
        this.connection = await MongoClient.connect(url)
        this.db = this.connection.db(dbName)
        this.collection = this.db.collection(collection)
        console.log('[MongoClient] Connected to ' + url + '/' + dbName + '/' + collection);
    }

    /**
     * Create a new document in collection
     * @param document -  document to insert
     * @returns the created document. 
     * Returned null if the connection to the collection has not initialized before calling 
     */
    async create(document){
        var insertResult = null
        if (this.collection != null){
            insertResult = await this.collection.insertOne(document)
        }
        return insertResult

    }
    /**
     * Find a document that contains the corresponding query
     * If multiple documents have the same query, return the first one found only
     * 
     * @param query- a field used to find the document
     * @returns the found document
     */
    async read(query){
        var readResult = null;
        if (this.collection != null){
            readResult = await this.collection.findOne(query);
        }
        return readResult;
    }

    /**
     * 
     * @param query field to find document
     * @returns array of found documents
     */
    async readmultiple(query)
    {
        var readResult = null;
        if (this.collection != null){
            readResult = await this.collection.find(query).toArray();
        }
        return readResult;
    }

    /**
    * Find all document in the collection
    * 
    * @returns array of documents in the collection
    */
    async readAll(){
        var resultArray = null;
        if (this.collection != null){
            resultArray = await this.collection.find().toArray();
        }
        return resultArray
    }

    
    /**
     * Update a field, given by query, of a document
     * If multiple documents have the same query, update the first one found only
     * @param query - field to find the document 
     * @param update - updated field to the document
     * @returns - updated document
     */
    async update(query, update){
        var updateResult = null;
        if (this.collection != null){
            updateResult = await this.collection.updateOne(query, {$set: update})
        }
        return updateResult
    }


    /**
     *  Delete a document with the given query 
     *  If multiple documents have the same query, delete the first one found only
     * @param query - field to find the document 
     * @returns the deleted document
     */
    async delete(query){
        var deleteResult = null;
        if  (this.collection != null){
           deleteResult = await this.collection.deleteOne(query)
        }
        return deleteResult
    }
    /**
     * Close the connection
     */
    close(){
        if (this.connection != null){
            this.connection.close();
        }
    }
}
