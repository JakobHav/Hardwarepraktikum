#!/bin/bash
echo "Creating virtual environment..."
python3 -m venv venv

echo "Activating virtual environment..."
source venv/bin/activate

echo "Installing required packages..."
pip install -r requirements.txt

echo ""
echo "Setup complete! Virtual environment created and packages installed."
echo ""
echo "To activate the environment in future sessions, run:"
echo "  source venv/bin/activate"
echo ""
echo "To run the visualization:"
echo "  python plot.py"
echo ""
