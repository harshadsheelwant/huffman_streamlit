import streamlit as st
import subprocess
import os

# Define supported file types
SUPPORTED_FILE_TYPES = ["text", "image", "video"]

# Streamlit Interface
st.title("File Compression using C")
st.write("Upload a file to compress it using the C compression program.")

# File Upload
uploaded_file = st.file_uploader("Choose a file", type=["txt", "png", "jpg", "mp4", "avi"])

# Check if file is uploaded
if uploaded_file is not None:
    file_path = os.path.join("/tmp", uploaded_file.name)

    # Save uploaded file to temp directory
    with open(file_path, "wb") as f:
        f.write(uploaded_file.read())
    
    st.write(f"File uploaded: {uploaded_file.name}")

    # Run the C compression executable
    if st.button("Compress File"):
        try:
            # Calling the C program through subprocess
            result = subprocess.run(["compress.c", file_path], capture_output=True, text=True)
            
            # Check if the compression was successful
            if result.returncode == 0:
                st.write("Compression successful!")
                st.write(result.stdout)  # Output from the C program
                
                # Provide a download link for the compressed file
                compressed_file = file_path + ".compressed"  # Assuming this is the output file
                with open(compressed_file, "rb") as f:
                    compressed_data = f.read()
                st.download_button(label="Download Compressed File", data=compressed_data, file_name=compressed_file)
            else:
                st.error("Compression failed!")
                st.write(result.stderr)  # Output errors, if any
        
        except Exception as e:
            st.error(f"An error occurred: {e}")