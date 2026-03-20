import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score

# Load dataset
data = pd.read_csv("dataset.txt")

print("Dataset loaded successfully")
print(data.head())

# Features (sensor values)
X = data.drop(["time_ms","status"], axis=1)

# Labels (motor state)
y = data["status"]

# Train/Test split
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)

# Train model
model = RandomForestClassifier(n_estimators=100)
model.fit(X_train, y_train)

print("Training completed")

# Prediction
predictions = model.predict(X_test)

# Accuracy
accuracy = accuracy_score(y_test, predictions)

print("Model Accuracy:", accuracy)

import joblib

# Save the trained model
joblib.dump(model, "motor_fault_model.pkl")

print("Model saved successfully!")