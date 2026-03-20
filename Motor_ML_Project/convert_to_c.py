import joblib
from micromlgen import port

# Load trained model
model = joblib.load("tinyml_model.pkl")

# Convert to C code
c_code = port(model)

# Save to header file
with open("motor_model.h", "w") as f:
    f.write(c_code)

print("Model converted to C successfully!")