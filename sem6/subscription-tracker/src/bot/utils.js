/**
 * Escape special characters for Telegram MarkdownV2.
 * Must be applied to any user-controlled string before embedding it
 * in a MarkdownV2 message to prevent formatting injection.
 *
 * Spec: https://core.telegram.org/bots/api#markdownv2-style
 */
exports.escapeMd = (text) =>
  String(text).replace(/[_*[\]()~`>#+\-=|{}.!\\]/g, '\\$&');