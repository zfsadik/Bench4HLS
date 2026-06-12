import logging
from .base import ModelBackend, ALPACA_PROMPT

logger = logging.getLogger(__name__)


class HuggingFaceBackend(ModelBackend):
    """Run inference via Hugging Face model."""

    def __init__(
        self,
        model_name: str,
        max_new_tokens: int = 30000,
        temperature: float = 1.0,
        use_4bit: bool = True,
        max_seq_length: int = 8192,
    ):
        self.model_name = model_name
        self.max_new_tokens = max_new_tokens
        self.temperature = temperature
        self._load(use_4bit, max_seq_length)

    def _load(self, use_4bit: bool, max_seq_length: int) -> None:
        try:
            self._load_unsloth(use_4bit, max_seq_length)
        except ImportError:
            logger.info("unsloth not found — using standard transformers")
            self._load_transformers(use_4bit)

    def _load_unsloth(self, use_4bit: bool, max_seq_length: int) -> None:
        from unsloth import FastLanguageModel  # type: ignore

        logger.info("Loading %s via unsloth (4bit=%s)", self.model_name, use_4bit)
        self.model, self.tokenizer = FastLanguageModel.from_pretrained(
            model_name=self.model_name,
            max_seq_length=max_seq_length,
            dtype=None,
            load_in_4bit=use_4bit,
        )
        FastLanguageModel.for_inference(self.model)
        self._backend = "unsloth"
        logger.info("unsloth model loaded")

    def _load_transformers(self, use_4bit: bool) -> None:
        import torch
        from transformers import AutoModelForCausalLM, AutoTokenizer, BitsAndBytesConfig  # type: ignore

        logger.info("Loading %s via transformers (4bit=%s)", self.model_name, use_4bit)

        quant_cfg = None
        if use_4bit:
            try:
                quant_cfg = BitsAndBytesConfig(
                    load_in_4bit=True,
                    bnb_4bit_compute_dtype=torch.float16,
                    bnb_4bit_use_double_quant=True,
                    bnb_4bit_quant_type="nf4",
                )
            except Exception as exc:
                logger.warning("bitsandbytes 4-bit config failed (%s) — loading in fp16", exc)

        self.tokenizer = AutoTokenizer.from_pretrained(self.model_name, trust_remote_code=True)
        self.model = AutoModelForCausalLM.from_pretrained(
            self.model_name,
            quantization_config=quant_cfg,
            torch_dtype=torch.float16 if not use_4bit else None,
            device_map="auto",
            trust_remote_code=True,
        )
        self.model.eval()
        self._backend = "transformers"
        logger.info("transformers model loaded")

    def generate(self, instruction: str) -> str:
        import torch

        prompt = ALPACA_PROMPT.format(instruction=instruction)
        inputs = self.tokenizer([prompt], return_tensors="pt").to(self.model.device)

        with torch.no_grad():
            output_ids = self.model.generate(
                **inputs,
                max_new_tokens=self.max_new_tokens,
                temperature=self.temperature,
                do_sample=self.temperature > 0,
            )

        generated_text = self.tokenizer.decode(output_ids[0], skip_special_tokens=True)
        return generated_text

    def close(self) -> None:
        import torch, gc

        del self.model
        del self.tokenizer
        gc.collect()
        if torch.cuda.is_available():
            torch.cuda.empty_cache()
