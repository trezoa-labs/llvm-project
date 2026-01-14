from trezoa_providers import *
from trezoa_types import TrezoaType, classify_trezoa_type


def summary_lookup(valobj, dict):
    # type: (SBValue, dict) -> str
    """Returns the summary provider for the given value"""
    trezoa_type = classify_trezoa_type(valobj.GetType())
    if trezoa_type == TrezoaType.PUBKEY:
        return PubkeySummaryProvider(valobj, dict)
    if trezoa_type == TrezoaType.ACCOUNT_INFO:
        return AccountInfoSummaryProvider(valobj, dict)
