# Example Python Code to Insert a Document 

from typing import Any, Dict, List, Optional
from pymongo import MongoClient
from pymongo.errors import PyMongoError

# Milestone 2 Enhancement:
## Added validation and query builder methods to centralize business logic
### Improves maintainability and separates database logic from the user interface
class AnimalShelter(object):
    """CRUD operations for Animal collection in MongoDB."""

    VALID_RESCUE_TYPES = {"water", "mountain", "disaster", "reset"}

    def __init__(
        self,
        username: str,
        password: str,
        host: str = "localhost",
        port: int = 27017,
        db_name: str = "AAC",
        collection_name: str = "animals",
    ):
        """Initialize MongoDB connection using authentication."""
        try:
            uri = f"mongodb://{username}:{password}@{host}:{port}/?authSource=AAC"
            self.client = MongoClient(uri)
            self.database = self.client[db_name]
            self.collection = self.database[collection_name]
        except PyMongoError as exc:
            raise RuntimeError(f"Failed to connect to MongoDB: {exc}") from exc

# Milestone 2 Enhancement:
## Validates user input to ensure only expected rescue types are processed

    def validate_rescue_type(self, filter_type: Optional[str]) -> str:
        """Validate the requested rescue filter and return a safe default if invalid."""
        if filter_type in self.VALID_RESCUE_TYPES:
            return filter_type
        return "reset"

# Milestone 2 Enhancement:
## Centralized query construction for rescue filters, which prevents business logic from being embedded in UI callbacks
    def build_rescue_query(self, filter_type: Optional[str]) -> Dict[str, Any]:
        """Return a MongoDB query for the selected rescue category."""
        validated_type = self.validate_rescue_type(filter_type)

        if validated_type == "water":
            return {
                "animal_type": "Dog",
                "breed": {"$in": ["Labrador Retriever Mix", "Chesapeake Bay Retriever", "Newfoundland"]},
                "sex_upon_outcome": "Intact Female",
                "age_upon_outcome_in_weeks": {"$gte": 26, "$lte": 156},
            }

        if validated_type == "mountain":
            return {
                "animal_type": "Dog",
                "breed": {"$in": ["German Shepherd", "Alaskan Malamute", "Old English Sheepdog", "Siberian Husky", "Rottweiler"]},
                "sex_upon_outcome": "Intact Male",
                "age_upon_outcome_in_weeks": {"$gte": 26, "$lte": 156},
            }

        if validated_type == "disaster":
            return {
                "animal_type": "Dog",
                "breed": {"$in": ["Doberman Pinscher", "German Shepherd", "Golden Retriever", "Bloodhound", "Rottweiler"]},
                "sex_upon_outcome": "Intact Male",
                "age_upon_outcome_in_weeks": {"$gte": 20, "$lte": 300},
            }

        return {}

 # Milestone 2 Enhancement:
## Provides reusable method to retrieve filtered data using validated queries
                                # Milestone 4 Enhancement:
                                ##Extended rescue-type query retrieval to support projections and result limits
    def read_by_rescue_type(
        self,
        filter_type: Optional[str],
        projection: Optional[Dict[str, int]] = None,
        limit: Optional[int] = None
    ) -> List[Dict[str, Any]]:
        """Read records using a validated rescue-type query with optional projection and limit."""
        query = self.build_rescue_query(filter_type)
        return self.read(query, projection=projection, limit=limit)
    
                                # Milestone 4 Enhancement:
                                ## Adds indexes on frequently queried dashboard fields to improve query performance, remove full collection scans
    def ensure_indexes(self) -> None:
        """Create indexes for fields commonly used in filtering and mapping."""
        try:
            self.collection.create_index("animal_type")
            self.collection.create_index("breed")
            self.collection.create_index("sex_upon_outcome")
            self.collection.create_index("age_upon_outcome_in_weeks")
            self.collection.create_index("location_lat")
            self.collection.create_index("location_long")
        except PyMongoError as exc:
            print(f"Index creation error: {exc}")

    # Create a method to return the next available record number for use in the create method     

    def create(self, data: Optional[Dict[str, Any]]) -> bool:
        if not data or not isinstance(data, dict):
            return False

        try:
            result = self.collection.insert_one(data)
            return result.acknowledged
        except PyMongoError:
            return False

                                # Milestone 4 Enhancement:
                                ## Enhances the read operation to support projections and controlled result limits, reducing unnecessary data transfer and memory usage
    def read(
        self,
        query: Optional[Dict[str, Any]],
        projection: Optional[Dict[str, int]] = None,
        limit: Optional[int] = None
    ) -> List[Dict[str, Any]]:
        if query is None or not isinstance(query, dict):
            return []

        try:
            cursor = self.collection.find(query, projection)

            if limit is not None:
                cursor = cursor.limit(limit)

            return list(cursor)
        except PyMongoError as exc:
            print(f"MongoDB read error: {exc}")
            return []
        
    # Create method to implement the U in CRUD.
    
    def update(self, query: Optional[Dict[str, Any]], new_values: Optional[Dict[str, Any]]) -> int:
        if query is None or not isinstance(query, dict):
            return 0
        if new_values is None or not isinstance(new_values, dict) or len(new_values) == 0:
            return 0

        try:
            # Use update_many to match the rubric language ("document(s)")
            result = self.collection.update_many(query, {"$set": new_values})
            return result.modified_count
        except PyMongoError:
            return 0

    # Create method to implement the D in CRUD.
    
    def delete(self, query: Optional[Dict[str, Any]]) -> int:
        if query is None or not isinstance(query, dict):
            return 0

        try:
            result = self.collection.delete_many(query)
            return result.deleted_count
        except PyMongoError:
            return 0