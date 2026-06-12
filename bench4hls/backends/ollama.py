import json
import logging
import urllib.request
import urllib.error
from .base import ModelBackend, ALPACA_SYSTEM

logger = logging.getLogger(__name__)

_DEFAULT_HOST = "http://localhost:11434"


class OllamaBackend(ModelBackend):
    """Run inference via local Ollama `/api/chat`."""

    def __init__(
        self,
        model_name: str,
        host: str = _DEFAULT_HOST,
        max_new_tokens: int = 30000,
        temperature: float = 1.0,
        timeout: int = 600,
    ):
        self.model_name = model_name
        self.host = host.rstrip("/")
        self.max_new_tokens = max_new_tokens
        self.temperature = temperature
        self.timeout = timeout
        self._check_connection()

    def _check_connection(self) -> None:
        url = f"{self.host}/api/tags"
        try:
            with urllib.request.urlopen(url, timeout=10) as resp:
                data = json.loads(resp.read())
            models = [m["name"] for m in data.get("models", [])]
            logger.info("Ollama reachable. Available models: %s", models)
            base = self.model_name.split(":")[0]
            if not any(base in m for m in models):
                logger.warning(
                    "Model '%s' not found in Ollama. Run: ollama pull %s",
                    self.model_name, self.model_name,
                )
        except urllib.error.URLError as exc:
            raise RuntimeError(
                f"Cannot reach Ollama at {self.host}. "
                "Start the server with: ollama serve"
            ) from exc

    def generate(self, instruction: str) -> str:
        url = f"{self.host}/api/chat"
        payload = {
            "model": self.model_name,
            "stream": False,
            "options": {
                "num_predict": self.max_new_tokens,
                "temperature": self.temperature,
            },
            "messages": [
                {"role": "system", "content": ALPACA_SYSTEM},
                {"role": "user",   "content": instruction},
            ],
        }
        body = json.dumps(payload).encode()
        req = urllib.request.Request(
            url,
            data=body,
            headers={"Content-Type": "application/json"},
            method="POST",
        )
        try:
            with urllib.request.urlopen(req, timeout=self.timeout) as resp:
                data = json.loads(resp.read())
            return data["message"]["content"]
        except urllib.error.URLError as exc:
            raise RuntimeError(f"Ollama request failed: {exc}") from exc
