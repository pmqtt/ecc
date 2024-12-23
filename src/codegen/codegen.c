#include "codegen.h"
#include <stdio.h>

void emit(FILE* file, char* t) {
    fprintf(file, "\t%s\n", t);
}

void emit_register(FILE* file, asm_register_t reg) {
    switch (reg) {
        case AX:
            fprintf(file, "%%eax");
            break;
        case DX:
            fprintf(file, "%%edx");
            break;
        case CL:
            fprintf(file, "%%cl");
            break;
        case R10:
            fprintf(file, "%%r10d");
            break;
        case R11:
            fprintf(file, "%%r11d");
            break;
    }
}

void emit_operand(FILE* file, operand_node* op) {
    switch (op->type) {
        case IMM:
            fprintf(file, "$%d", op->operand.imm->i);
            break;
        case REG:
            emit_register(file, op->operand.reg->reg);
            break;
        case STACK:
            fprintf(file, "-%d(%%rbp)", op->operand.stack->offset);
        default:
            break;
    }
}

void emit_mov_instr(FILE* file, asm_move_node* mov) {
    fprintf(file, "\tmov");
    switch (mov->size) {
        case ASM_LONG:
            fprintf(file, "l ");
            break;
        case ASM_BYTE:
            fprintf(file, "b ");
            break;
    }
    emit_operand(file, mov->src);
    fprintf(file, ", ");
    emit_operand(file, mov->dest);
    fprintf(file, "\n");
}

void emit_unary_op(FILE* file, asm_unary_op op) {
    switch (op) {
        case ASM_NOT:
            fprintf(file, "notl");
            break;
        case ASM_NEG:
            fprintf(file, "negl");
            break;
    }
}

void emit_unary_instr(FILE* file, asm_unary_node* unary) {
    fprintf(file, "\t");
    emit_unary_op(file, unary->op);
    fprintf(file, " ");
    emit_operand(file, unary->dest);
    fprintf(file, "\n");
}

void emit_binary_op(FILE* file, asm_binary_op op) {
    switch (op) {
        case ASM_ADD:
            fprintf(file, "addl");
            break;
        case ASM_SUB:
            fprintf(file, "subl");
            break;
        case ASM_MULT:
            fprintf(file, "imull");
            break;
        case ASM_AND:
            fprintf(file, "andl");
            break;
        case ASM_OR:
            fprintf(file, "orl");
            break;
        case ASM_XOR:
            fprintf(file, "xor");
            break;
        case ASM_LEFT_SHIFT:
            fprintf(file, "shll");
            break;
        case ASM_RIGHT_SHIFT:
            fprintf(file, "shrl");
            break;
        default:
            break;
    }
}

void emit_binary_instr(FILE* file, asm_binary_node* binary) {
    fprintf(file, "\t");
    emit_binary_op(file, binary->op);
    fprintf(file, " ");
    emit_operand(file, binary->src);
    fprintf(file, ", ");
    emit_operand(file, binary->dest);
    fprintf(file, "\n");
}

void emit_idiv_instr(FILE* file, asm_idiv_node* idiv) {
    fprintf(file, "\tidiv ");
    emit_operand(file, idiv->divisor);
    fprintf(file, "\n");
}

void emit_return_instr(FILE* file) {
    emit(file, "movq %rbp, %rsp");
    emit(file, "popq %rbp");
    emit(file, "ret");
}

void emit_cdq_instr(FILE* file) {
    emit(file, "cdq");
}

void emit_stackalloc_instr(FILE* file, asm_stackalloc_node* stackalloc) {
    fprintf(file, "\tsubq $%d, %%rsp\n", stackalloc->n_bytes);
}

void emit_instruction(FILE* file, asm_instruction_node* instr) {
    switch (instr->type) {
        case INSTR_MOV:
            emit_mov_instr(file, instr->instruction.mov);
            break;
        case INSTR_UNARY:
            emit_unary_instr(file, instr->instruction.unary);
            break;
        case INSTR_RET:
            emit_return_instr(file);
            break;
        case INSTR_STACKALLOC:
            emit_stackalloc_instr(file, instr->instruction.stackalloc);
            break;
        case INSTR_BINARY:
            emit_binary_instr(file, instr->instruction.binary);
            break;
        case INSTR_IDIV:
            emit_idiv_instr(file, instr->instruction.idiv);
            break;
        case INSTR_CDQ:
            emit_cdq_instr(file);
            break;
    }
}

void emit_function(FILE* file, asm_function_node* function) {
    fprintf(file, ".globl %s\n", function->identifier);
    fprintf(file, "%s:\n", function->identifier);
    emit(file, "pushq %rbp");
    emit(file, "movq %rsp, %rbp");
    for (int i = 0; i < function->instructions->len; i++) {
        asm_instruction_node* instr = (asm_instruction_node*)list_get(function->instructions, i);
        emit_instruction(file, instr);
    }
}

void emit_program(FILE* file, asm_program_node* program) {
    emit_function(file, program->function);
    fprintf(file, "\n");
    fprintf(file, ".section .note.GNU-stack,\"\",@progbits\n");
}