from abc import ABC, abstractmethod
from typing import List, Dict


ALPACA_SYSTEM = (
    "Your role is to act as an expert in HLS C++ code development. You must thoroughly explore each question through a systematic and"
    "deliberate thinking process—engaging in cycles of analysis, summarization, exploration, reassessment, reflection, backtracing, and"
    "iteration to develop well-considered solutions. Based on the provided instructions, you are expected to generate precise, optimized,"
    "and accurate synthesizable HLS C++ code that meets the following requirements.\n"
    "1. Output only synthesizable C++ code (no extra text or explanation).\n"
    "2. Include necessary headers (e.g. ap_int.h).\n"
    "3. The top-level function signature must be exactly as given in the instruction.\n"
    "4. Insert suitable HLS pragmas.\n"
    "5. Self-contained: do not rely on external libraries or files beyond standard headers.\n"
)

ALPACA_PROMPT = (
    ALPACA_SYSTEM
    + "\n### Instruction:\n{instruction}\n\n"
    "### Input:\n\n"
    "### Response:\n"
)


class ModelBackend(ABC):
    """Abstract base for inference backends."""

    @abstractmethod
    def generate(self, instruction: str) -> str:
        """Return model response for one HLS task."""

    def close(self) -> None:
        """Optional cleanup (unload model, close connections, etc.)."""
