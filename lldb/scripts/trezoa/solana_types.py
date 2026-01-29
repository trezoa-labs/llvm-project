import re


class TrezoaType(object):
    PUBKEY = "Pubkey"
    ACCOUNT_INFO = "AccountInfo"

PUBKEY_REGEX = re.compile(r"^(trezoa_program::pubkey::Pubkey)")
ACCOUNT_INFO_REGEX = re.compile(r"^(trezoa_program::account_info::AccountInfo)")

trezoa_TYPE_TO_REGEX = {
    TrezoaType.PUBKEY: PUBKEY_REGEX,
    TrezoaType.ACCOUNT_INFO: ACCOUNT_INFO_REGEX,
}

def classify_trezoa_type(type):
    for ty, regex in trezoa_TYPE_TO_REGEX.items():
        if regex.match(type.name):
            return ty
