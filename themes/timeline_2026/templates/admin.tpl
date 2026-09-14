{* timeline_2026 - admin view.
   No wrapper here: theme.tpl already provides <main class="tl-main">, and
   Gallery2's own admin markup brings its own panels and left nav. *}
<div class="tl-panel tl-panel--host">
  {include file="gallery:`$theme.adminTemplate`" l10Domain=$theme.adminL10Domain}
</div>
