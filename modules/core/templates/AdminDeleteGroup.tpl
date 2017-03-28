{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Delete A Group"} </h2>
</div>

<div class="gbBlock">
  <h3> {g->text text="Are you sure?"} </h3>

  <p class="giDescription">
    {capture name="group"}<strong>{$AdminDeleteGroup.group.groupName}</strong>{/capture}
    {g->text text="This will completely remove %s from Gallery.  There is no undo!"
     arg1=$smarty.capture.group}
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input type="hidden"
   name="{g->formVar var="groupId"}" value="{$AdminDeleteGroup.group.id}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][delete]"}" value="{g->text text="Delete"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
